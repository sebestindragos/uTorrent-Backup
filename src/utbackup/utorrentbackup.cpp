#include "stdafx.h"
#include "utorrentbackup.h"
#include "utbackup/propertypool/IPropertyPool.h"
#include "bencode/bEncodeManager.h"
#include "common/util/FilePath.h"
#include "common/os/Registry.h"
#include "common/util/StringUtil.h"
#include "common/util/File.h"
#include "utbackup/UIManager.h"
#include "common/os/WinTaskbar.h"

#define UTB_DEF_ALT_SUBFOLDER       L"Alternate"

#define UTB_REG_PROPERTIES_KEY      L"Software\\uTorrent Backup\\Properties"

uTorrentBackup::uTorrentBackup(CWindow aFrameWnd)
  : mFrameWnd(aFrameWnd), mWorkerFunc(nullptr), 
    mWorkerHadException(false)
{
  // initialize the property pool
  try
  {
    ReadPersistentProps();
    ComputeDynamicProps();
  }
  CATCH_EVERYTHING;
}

uTorrentBackup::~uTorrentBackup()
{
  try
  {
    SavePersistentProps();
  }
  CATCH_EVERYTHING;
}

IPropertyPool & uTorrentBackup::GetPropertyPool()
{
  return mPropertyPool;
}

DWORD uTorrentBackup::WorkerThreadProc(LPVOID aData)
{
  uTorrentBackup * self = nullptr;
  try
  {
    self = reinterpret_cast<uTorrentBackup *>(aData);

    // run the worker function
    self->mWorkerFunc();
  }
  CATCH_EVERYTHING;
  
  self->mThreadHandle = NULL;
  return 0;
}

void uTorrentBackup::StartWorkerThread()
{
  try
  {
    mWorkerHadException = false;
    if (mWorkerFunc)
    {
      mThreadHandle = ::CreateThread(NULL, 0, 
        reinterpret_cast<LPTHREAD_START_ROUTINE>(&uTorrentBackup::WorkerThreadProc), 
        reinterpret_cast<LPVOID>(this), 0, NULL);
    }
  }
  CATCH_EVERYTHING;
}

bool uTorrentBackup::Backup()
{
  try
  {
    mWorkerFunc = std::bind(&uTorrentBackup::DoBackup, this);
    StartWorkerThread();
  }
  CATCH_EVERYTHING

  return true;
}

bool uTorrentBackup::DoBackup()
{
  UIManager::Get().DisplayStatusMessage(L"Working ...");

  FilePath backupPath(mPropertyPool.GetProperty(UTB_BACKUPPATH_PROP));
  FilePath utPath(mPropertyPool.GetProperty(UT_PATH_PROP));
  utPath.EnsureEndingBackslash();
  FilePath utAltPath(mPropertyPool.GetProperty(UT_ALTERNATEFOLDERPATH_PROP));
  utAltPath.EnsureEndingBackslash();
  FilePath alternateBackupPath(backupPath.GetParent() + mPropertyPool.GetProperty(UTB_BACKUPALTERNATEPATH_PROP));
  alternateBackupPath.EnsureEndingBackslash();

  // create the backup directory
  FilePath dir = backupPath.GetParent();
  ::CreateDirectory(dir.Get().c_str(), NULL);

  // Build the .utb file which will be used later for the restore process.
  bEncodeManager bManager;

  // utb props
  bManager.AddChildItem(UTB_BACKUPPACKFILES_PROP, 
    mPropertyPool.GetProperty(UTB_BACKUPPACKFILES_PROP)); 
  bManager.AddChildItem(UTB_BACKUPPATH_PROP, backupPath.Get()); 

  if (mPropertyPool.GetProperty(UTB_BACKUPALTERNATE_PROP) == PROP_TRUE && 
      mPropertyPool.GetProperty(UT_ALTERNATEFOLDER_PROP) == PROP_TRUE)
    bManager.AddChildItem(UTB_BACKUPALTERNATE_PROP, PROP_TRUE);
  else
    bManager.AddChildItem(UTB_BACKUPALTERNATE_PROP, PROP_FALSE);
  
  bManager.AddChildItem(UTB_BACKUPALTERNATEPATH_PROP, alternateBackupPath.Get());
  bManager.AddChildItem(UTB_CREATE_VERSION, UTORRENT_BACKUP_VERSION);

  // ut props
  bManager.AddChildItem(UT_ALTERNATEFOLDERPATH_PROP, utAltPath.Get());

  bool packFiles = (mPropertyPool.GetProperty(UTB_BACKUPPACKFILES_PROP) == PROP_TRUE);
  if (!packFiles)
  {
    DoBackupUnpacked(utPath, backupPath, utAltPath, alternateBackupPath);
  }
  else if (packFiles)
  {
    DoBackupPacked(utPath, utAltPath, bManager);
  }
  else
  {
   throw Exception(mFrameWnd, L"Undefined backup mode.");
  }

  wstring encodedData;
  bManager.Encode(encodedData);

  FileUtil::File utbFile(backupPath.Get(), true);

  if (!utbFile.IsValid())
    throw Exception(mFrameWnd, L"Could not create file");
  if (!utbFile.Write(encodedData))
    throw Exception(mFrameWnd, L"Could not write file");

  UIManager::Get().DisplayStatusMessage(L"Backup complete");

  return true;
}

bool uTorrentBackup::DoBackupPacked(const FilePath & aUtPath,
                                    const FilePath & aAltPath,
                                    bEncodeManager & aBManager)
{
  vector<wstring> fileNames;
  vector<wstring> fileContents;

  aBManager.AddChildItem(L"Files", L"", bEncodeItem::Dictionary, false);

  auto GetFiles = [&](const FilePath & aRoot, const wstring & aFileTypes)
  {
    CFindFile find;
    FilePath from(aRoot);
    from.Append(aFileTypes);
    find.FindFile(from.Get().c_str());
    if (find.m_bFound)
    {
      do 
      {
        fileNames.push_back(find.GetFileName().GetString());
        File file(find.GetFilePath().GetString());
        fileContents.push_back(file.Read());
      } while (find.FindNextFile());
    }
  };

  GetFiles(aUtPath, L"*.torrent");
  GetFiles(aUtPath, L"*.dat");
  GetFiles(aUtPath, L"*.old");

  for (size_t i = 0; i < fileNames.size(); i++)
  {
    aBManager.AddChildItem(fileNames[i], fileContents[i]);
  }
  aBManager.CloseItem();

  // pack alternate files
  if (mPropertyPool.GetProperty(UTB_BACKUPALTERNATE_PROP) == PROP_TRUE &&
      mPropertyPool.GetProperty(UT_ALTERNATEFOLDER_PROP) == PROP_TRUE)
  {
    aBManager.AddChildItem(L"Alternate Files", L"", bEncodeItem::Dictionary, false);

    fileNames.clear();
    fileContents.clear();

    GetFiles(aAltPath, L"*.torrent");

    for (size_t i = 0; i < fileNames.size(); i++)
    {
      aBManager.AddChildItem(fileNames[i], fileContents[i]);
    }

    aBManager.CloseItem();
  }

  return true;
}

bool uTorrentBackup::DoBackupUnpacked(const FilePath & aUtPath, const FilePath & aBackupPath, 
                                      const FilePath & aAltPath, const FilePath & aAltBackupPath)
{
  if (!DoCopyFiles(aUtPath, aBackupPath.GetParent()))
    return false;

  if (mPropertyPool.GetProperty(UTB_BACKUPALTERNATE_PROP) == PROP_TRUE &&
      mPropertyPool.GetProperty(UT_ALTERNATEFOLDER_PROP) == PROP_TRUE)
  {
    if (!DoCopyFiles(aAltPath, aAltBackupPath))
      return false;
  }

  return true;
}

bool uTorrentBackup::DoCopyFiles(const FilePath & aSourcePath, const FilePath & aDestPath)
{
  if (aSourcePath.Get().size() == 0 || aDestPath.Get().size() == 0)
    return false;

  wstring currentDir;
  currentDir.resize(::GetCurrentDirectory(0, NULL) + 1);
  ::GetCurrentDirectory((DWORD)currentDir.size(), &currentDir[0]);
  ::SetCurrentDirectory(aSourcePath.Get().c_str());

  SHFILEOPSTRUCT shop;
  ZeroMemory(&shop, sizeof(SHFILEOPSTRUCT));

  shop.hwnd = mFrameWnd;
  shop.wFunc = FO_COPY;
  shop.pFrom = L"*.torrent\0*.dat\0*.old\0\0";
  shop.pTo = aDestPath.Get().c_str();
  shop.fFlags = FOF_NOCONFIRMMKDIR | FOF_SIMPLEPROGRESS;

  int ret = ::SHFileOperation(&shop);
  ::SetCurrentDirectory(currentDir.c_str());
  if (ret && shop.fAnyOperationsAborted == 0)
    return false;
  return true;
}

bool uTorrentBackup::Restore(bool aWaitForOperation)
{
  try
  {
    mWorkerFunc = std::bind(&uTorrentBackup::DoRestore, this);
    StartWorkerThread();
    if (aWaitForOperation)
      ::WaitForSingleObject(mThreadHandle, INFINITE);
  }
  CATCH_EVERYTHING;

  return true;
}

bool uTorrentBackup::DoRestore()
{
  UIManager::Get().DisplayStatusMessage(L"Working ...");

  // decode the utb file
  FilePath utbFilePath(mPropertyPool.GetProperty(UTB_BACKUPPATH_PROP));

  FileUtil::File utbFile(utbFilePath.Get());
  if (!utbFile.IsValid())
    throw Exception(NULL, utbFilePath.Get() + L" file was not found.");

  bEncodeManager bManager;
  if (!bManager.Decode2(utbFile.Read()))
    throw Exception(NULL, L"Malformed file.");

  FilePath backupFolderPath = FilePath(bManager.GetItemValue(UTB_BACKUPPATH_PROP)).GetParent();
  backupFolderPath.EnsureEndingBackslash();
  FilePath restorePath(mPropertyPool.GetProperty(UT_PATH_PROP));
  restorePath.EnsureEndingBackslash();
  FilePath alternateFolderPath = bManager.GetItemValue(UT_ALTERNATEFOLDERPATH_PROP);
  alternateFolderPath.EnsureEndingBackslash();

  if (bManager.GetItemValue(UTB_BACKUPPACKFILES_PROP) == PROP_FALSE)
  {
    DoRestoreUnpacked(restorePath, backupFolderPath, alternateFolderPath, bManager);
  }
  else if (bManager.GetItemValue(UTB_BACKUPPACKFILES_PROP) == PROP_TRUE)
  {
    DoRestorePacked(restorePath, alternateFolderPath, bManager);
  }

  UIManager::Get().DisplayStatusMessage(L"Restore complete");
  return true;
}

void uTorrentBackup::DoRestoreUnpacked(const FilePath & aUtPath, 
                                       const FilePath & aBackupPath, 
                                       const FilePath & aAltPath, 
                                       bEncodeManager & aBManager)
{
  DoCopyFiles(aBackupPath, aUtPath);

  bool restoreAlternate = 
    aBManager.GetItemValue(UTB_BACKUPALTERNATE_PROP) == PROP_TRUE;
  if (restoreAlternate)
  {
    FilePath altBkpFolderPath = aBManager.GetItemValue(UTB_BACKUPALTERNATEPATH_PROP);
    altBkpFolderPath.EnsureEndingBackslash();
    
    DoCopyFiles(altBkpFolderPath, aAltPath);
  }
}

void uTorrentBackup::DoRestorePacked(const FilePath & aUtPath, 
                                     const FilePath & aAltRestorePath,
                                     bEncodeManager & aBManager)
{
  auto UnpackFiles = [&](const wstring & aFilesItem, const FilePath & aRestorePath)
  {
    bEncodeItem * filesItem = aBManager.GetItem(aFilesItem, bEncodeItem::Dictionary);
    if (!filesItem)
      throw Exception(mFrameWnd, L"Corrupted utb file");
    auto it = filesItem->ChildrenBegin();
    auto end = filesItem->ChildrenEnd();
    for (; it != end; ++it)
    {
      FilePath restoreFilePath = aRestorePath;
      restoreFilePath.Append((*it)->GetLabel());
      File file(restoreFilePath.Get(), true);
      file.Write((*it)->GetValue());
    }
  };

  UnpackFiles(L"Files", aUtPath);

  bool restoreAlternate = 
    aBManager.GetItemValue(UTB_BACKUPALTERNATE_PROP) == PROP_TRUE;
  if (restoreAlternate)
  {
    UnpackFiles(L"Alternate Files", aAltRestorePath);
  }
}

void uTorrentBackup::ReadPersistentProps()
{
  Registry reg(HKEY_CURRENT_USER, UTB_REG_PROPERTIES_KEY);

  mPropertyPool.SetProperty(UTB_BACKUPPATH_PROP, reg.ReadString(UTB_BACKUPPATH_PROP));
  mPropertyPool.SetProperty(UTB_BACKUPPACKFILES_PROP, reg.ReadString(UTB_BACKUPPACKFILES_PROP));
  mPropertyPool.SetProperty(UTB_BACKUPALTERNATE_PROP, reg.ReadString(UTB_BACKUPALTERNATE_PROP));
}

void uTorrentBackup::SavePersistentProps()
{
  Registry reg(HKEY_CURRENT_USER, UTB_REG_PROPERTIES_KEY, true);

  reg.WriteString(UTB_BACKUPPATH_PROP, mPropertyPool.GetProperty(UTB_BACKUPPATH_PROP));
  reg.WriteString(UTB_BACKUPPACKFILES_PROP, mPropertyPool.GetProperty(UTB_BACKUPPACKFILES_PROP));
  reg.WriteString(UTB_BACKUPALTERNATE_PROP, mPropertyPool.GetProperty(UTB_BACKUPALTERNATE_PROP));
}

void uTorrentBackup::ComputeDynamicProps()
{
  CString appdataPath;
  ::SHGetSpecialFolderPath(mFrameWnd, appdataPath.GetBuffer(1024), CSIDL_APPDATA, FALSE);
  appdataPath.ReleaseBuffer();

  FilePath utPath(appdataPath.GetString());
  utPath.Append(L"utorrent");
  utPath.EnsureEndingBackslash();

  mPropertyPool.SetProperty(UT_PATH_PROP, utPath.Get());
  mPropertyPool.SetProperty(UTB_BACKUPALTERNATEPATH_PROP, UTB_DEF_ALT_SUBFOLDER);
  
  FilePath settingsFilePath(utPath);
  settingsFilePath.Append(Globals::kUtSettingsFile);
  FileUtil::File settingsFile(settingsFilePath.Get());

  bEncodeManager bManager;
  bManager.Decode2(settingsFile.Read());

  FilePath utAltPath = bManager.GetItemValue(L"dir_torrent_files");
  utAltPath.EnsureEndingBackslash();
  mPropertyPool.SetProperty(UT_ALTERNATEFOLDERPATH_PROP, utAltPath.Get());

  wstring backupAltProp = 
    bManager.GetItemValue(L"dir_torrent_files_flag") == L"1"? PROP_TRUE : PROP_FALSE;
  mPropertyPool.SetProperty(UT_ALTERNATEFOLDER_PROP, backupAltProp);

  mPropertyPool.SetProperty(UTB_CREATE_VERSION, UTORRENT_BACKUP_VERSION);
}

bool uTorrentBackup::HasException()
{
  return mWorkerHadException;
}

void uTorrentBackup::RaiseException()
{
  throw mThreadException;
}
