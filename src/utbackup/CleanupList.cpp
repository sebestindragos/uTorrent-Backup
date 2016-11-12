#include "stdafx.h"
#include "CleanupList.h"
#include "common/util/UIUtil.h"
#include "bencode/bEncodeManager.h"
#include "bencode/bEncodeItem.h"
#include "propertypool/IPropertyPool.h"
#include "common/util/File.h"
#include "UIManager.h"
#include "common/util/WorkerThread.h"
#include "common/os/DpiUtil.h"

//----------------------------------------------------------------------------
// Class definition - public members.
CleanupList::CleanupList(IPropertyPool & aPropPool)
 : mPropPool(aPropPool), mUIManager(UIManager::Get()), mWorker(new WorkerThread)
{
  mWorker->AddObserver(this);
}

CleanupList::~CleanupList()
{

}

void CleanupList::Attach(HWND aListWnd)
{
  CWindowImpl::SubclassWindow(aListWnd);
  ListView_SetExtendedListViewStyle(*this, LVS_EX_CHECKBOXES | LVS_EX_FULLROWSELECT);

  LVCOLUMN lvCol = {sizeof(LVCOLUMN)};
  lvCol.mask = LVCF_TEXT | LVCF_WIDTH;
  lvCol.pszText = L"";
  lvCol.cchTextMax = 0;
  lvCol.cx = DpiUtil::AdjustValueToCurrentDpi(20);
  InsertColumn(0, &lvCol);

  lvCol.pszText = L"File path";
  lvCol.cchTextMax = 8;
  lvCol.cx = 300;
  InsertColumn(1, &lvCol);

  lvCol.pszText = L"Type";
  lvCol.cchTextMax = 4;
  lvCol.cx = 100;
  InsertColumn(2, &lvCol);

  if (WinVer::IsWindowsVista())
  {
    UIUtil::SetListViewExplorerTheme(aListWnd);
    CHeaderCtrl header = GetHeader();
    ::SetWindowLong(header, GWL_STYLE, header.GetStyle() | HDS_CHECKBOXES);

    HDITEM hdrItem;
    if (header.GetItem(0, &hdrItem))
    {
      hdrItem.mask |= HDI_FORMAT;
      hdrItem.fmt |= HDF_CHECKBOX | HDF_FIXEDWIDTH;

      header.SetItem(0, &hdrItem);
    }
  }
}

void CleanupList::ScanFiles()
{
  try
  {
    EnableUI(false);
    mWorker->DoWorkAsync(std::bind(&CleanupList::DoScanFiles, this));
  }
  CATCH_EVERYTHING;
}

void CleanupList::CleanFiles()
{
  try
  {
    EnableUI(false);
    mWorker->DoWorkAsync(std::bind(&CleanupList::DoCleanFiles, this));    
  }
  CATCH_EVERYTHING
}

void CleanupList::NotifyWorkDone()
{
  EnableUI(true);
}

//----------------------------------------------------------------------------
// Class definition - private members.
void CleanupList::DoScanFiles()
{
  mUIManager.DisplayStatusMessage(L"Scanning files ...");
  DeleteAllItems();

  // get a list of the active torrents
  FilePath resumeFilePath(mPropPool.GetProperty(UT_PATH_PROP) + Globals::kUtResumeFile);
  FileUtil::File resumeFile(resumeFilePath.Get());
  if (!resumeFile.IsValid())
    throw Exception(NULL, resumeFilePath.Get() + L" could not be found.");
  wstring encodedData(resumeFile.Read());

  bEncodeManager bManager;
  bManager.Decode2(encodedData);

  // get utorrent settings folder path
  FilePath utPath = mPropPool.GetProperty(UT_PATH_PROP);
  
  vector<wstring> junkFiles;

  // find leftover files
  bEncodeItem * root = bManager.GetRootItem();
  auto it = root->ChildrenBegin();
  auto end = root->ChildrenEnd();
  size_t torrentCount = root->GetChildrenCount();
  for (; it != end; ++it)
  {
    mUIManager.SetProgress((int)(torrentCount - (end - it) + 1), (int)torrentCount);
    bEncodeItem * crt = *it;
    
    bEncodeItem * downloadPath = crt->GetChild(L"path");
    if (downloadPath)
    {
      FilePath torrentPath = (*it)->GetLabel();
      if (!torrentPath.IsAbsolute())
        torrentPath = utPath + torrentPath;

      // get the list of files from the torrent file.
      vector<wstring> torrentFiles = GetFilesFromTorrent(torrentPath);

      // get the list of files from the download location.
      vector<wstring> dirFiles;
      FileUtil::ListDirectoryContents(downloadPath->GetValue(), dirFiles, true, L"*", false, true);

      // list the extra files from the download folder
      for_each(dirFiles.begin(), dirFiles.end(), 
      [&](const wstring & aFile)
      {
        if (std::find(torrentFiles.begin(), torrentFiles.end(), aFile) == torrentFiles.end())
        {
          FilePath itemPath = downloadPath->GetValue();
          itemPath.Append(aFile);

          junkFiles.push_back(itemPath.Get());
        }
      });
    }
  }

  // get the list of .torrent files on disk
  vector<wstring> inactiveTorrentFiles;
  FileUtil::ListDirectoryContents(utPath.Get(), inactiveTorrentFiles, false, L"torrent");
  if (mPropPool.GetProperty(UT_ALTERNATEFOLDER_PROP) == PROP_TRUE)
  {
    FilePath altPath = mPropPool.GetProperty(UT_ALTERNATEFOLDERPATH_PROP);
    FileUtil::ListDirectoryContents(altPath.Get(), inactiveTorrentFiles, false, L"torrent", true);
  }

  // remove active ones
  it = root->ChildrenBegin();
  for (; it != end; ++it)
  {
    auto found = std::find(inactiveTorrentFiles.begin(), inactiveTorrentFiles.end(), (*it)->GetLabel());
    if ( found != inactiveTorrentFiles.end() )
      inactiveTorrentFiles.erase(found);
  }

  // expand relative paths
  for (size_t i = 0; i < inactiveTorrentFiles.size(); ++i)
  {
    FilePath itemPath = inactiveTorrentFiles[i];
    if (!itemPath.IsAbsolute())
    {
      itemPath = utPath;
      itemPath.Append(inactiveTorrentFiles[i]);
      inactiveTorrentFiles[i] = itemPath.Get(); 
    }
  }

  // merge files
  junkFiles.insert(junkFiles.end(), inactiveTorrentFiles.begin(), inactiveTorrentFiles.end());

  // populate the list
  Populate(junkFiles);

  CString msg;
  msg.Format(L"Scanning complete. Found %d file(s).", junkFiles.size());
  mUIManager.DisplayStatusMessage(msg.GetString());
}

vector<wstring> CleanupList::GetFilesFromTorrent(const FilePath & aTorrentPath)
{
  File torrent(aTorrentPath.Get());
  if (!torrent.IsValid())
    return vector<wstring>();

  wstring content = torrent.Read();

  bEncodeManager bMgr;
  bMgr.Decode2(content);

  // get the info item which stores the files.
  vector<wstring> torrentFiles;
  bEncodeItem * info = bMgr.GetItem(L"info", bEncodeItem::Dictionary);
  if (info)
  {
    bEncodeItem * files = info->GetChild(L"files", bEncodeItem::List);
    if (files)
    {
      for (auto it = files->ChildrenBegin(); it != files->ChildrenEnd(); ++it)
      {
        bEncodeItem * crt = (*it)->GetChild(L"path", bEncodeItem::List);
        if (crt)
        {
          FilePath itemPath;
          for_each(crt->ChildrenBegin(), crt->ChildrenEnd(), [&](bEncodeItem * aPart)
          {
            itemPath.Append(aPart->GetLabel());
          });
          torrentFiles.push_back(itemPath.Get());
        }
        
      }
    }
  }

  return torrentFiles;
}

void CleanupList::DoCleanFiles()
{
  mUIManager.DisplayStatusMessage(L"Cleaning files ...");

  int count = GetItemCount();
  double totalSize = 0;
  int numFilesCleaned = 0;
  CString msg;
  for (int i = 0; i < count; ++i)
  {
    mUIManager.SetProgress(i + 1, count);
    BOOL checked = GetCheckState(i);
    if (checked)
    {
      CString filePath;
      GetItemText(i, 1, filePath);

      File file(filePath.GetString());
      double size = (double)file.GetSize() / 1000000;
      file.Close();

      BOOL fileDeleted = TRUE;
#ifndef _DEBUG
      fileDeleted = ::DeleteFile(filePath.GetString());
#endif
      if (fileDeleted)
      {
        totalSize += size;
        numFilesCleaned++;
      }
    }
  }
  msg.Format(L"Cleaned %d file(s). Saved %.2f MB of space.", numFilesCleaned, totalSize);
  mUIManager.DisplayStatusMessage(msg.GetString());
}

LRESULT CleanupList::OnHeaderCheck(int, LPNMHDR aNMHDR, BOOL &)
{
  try
  {
    LPNMHEADER pnmHeader = (LPNMHEADER)aNMHDR;

    if (pnmHeader->pitem->mask & HDI_FORMAT && 
      pnmHeader->pitem->fmt & HDF_CHECKBOX) 
    {
      for (int i = 0; i < GetItemCount(); ++i)
        SetCheckState(i, !(pnmHeader->pitem->fmt & HDF_CHECKED));
      SetHeaderCheckbox();
      return 1;
    }
  }
  CATCH_EVERYTHING

  return 0;
}

void CleanupList::SetHeaderCheckbox(void) 
{
  try
  {
    BOOL checked = TRUE;
    for (int i = 0; i < GetItemCount() && checked; ++i)
    {
      if (!GetCheckState(i)) 
        checked = FALSE;
    }

    // We need to get the current format of the header
    // and set or remove the HDF_CHECKED flag
    CHeaderCtrl header = GetHeader();
    HDITEM hdi = { 0 };
    hdi.mask = HDI_FORMAT;
    header.GetItem(0, &hdi);
    if (checked)
    {
      hdi.fmt |= HDF_CHECKED;
    } 
    else
    {
      hdi.fmt &= ~HDF_CHECKED;
    }
    header.SetItem(0, &hdi);
  }
  CATCH_EVERYTHING
}

LRESULT CleanupList::OnItemChanged(int, LPNMHDR aNMHDR, BOOL &)
{
  try
  {
    LPNMLISTVIEW pnmlv = (LPNMLISTVIEW)aNMHDR;
    if (pnmlv->uChanged & LVIF_STATE)
      SetHeaderCheckbox();

    if ( (pnmlv->uOldState & LVIS_STATEIMAGEMASK) != (pnmlv->uNewState & LVIS_STATEIMAGEMASK) &&
		 GetItemState(pnmlv->iItem, LVIS_SELECTED) & LVIS_SELECTED )
    {
      // also set check for all selected items
      BOOL checked = GetCheckState(pnmlv->iItem);
      for (int i = 0; i < GetItemCount(); ++i)
      {
        if (GetItemState(i, LVIS_SELECTED) & LVIS_SELECTED)
          SetCheckState(i, checked);
      }
    }

  }
  CATCH_EVERYTHING
  return 0;
}

void CleanupList::Populate(const vector<wstring> & aFilesList)
{
  for (size_t i = 0; i < aFilesList.size(); ++i)
  {
    FilePath itemPath(aFilesList[i]);

    AddItem((int)i, 0, L"");
    AddItem((int)i, 1, itemPath.Get().c_str(), 0);
    AddItem((int)i, 2, itemPath.GetExtension().c_str());
    SetCheckState((int)i, TRUE);
  }
}

void CleanupList::EnableUI(bool aEnable)
{
  CWindow parent(GetParent());

  CButton btn(parent.GetDlgItem(IDC_BUT_SCAN));
  btn.EnableWindow(aEnable);

  btn.Detach();
  btn.Attach(parent.GetDlgItem(IDC_BUT_CLEAN));
  btn.EnableWindow(aEnable);
}
