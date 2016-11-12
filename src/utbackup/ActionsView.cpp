#include "stdafx.h"
#include "ActionsView.h"
#include "utorrentbackup.h"
#include "propertypool/IPropertyPool.h"
#include "bencode/bEncodeManager.h"
#include "common/util/File.h"
#include "utbackup/propertypool/IPropertyObserver.h"
#include "utbackup/UIManager.h"
#include "utbackup/backupinfo/BackupInfoDlg.h"

#define UTB_DEF_FILENAME       L"restore.utb"
#define UTB_DEF_FILE_EXTENSION L"utb"

ActionsView::ActionsView(HWND aParent, uTorrentBackup & aUtBackup)
  : mUtBackup(aUtBackup)
{
  mPropPool = &aUtBackup.GetPropertyPool();
  mPropPool->AddObserver(this);

  CDialogImpl::Create(aParent);
}

LRESULT ActionsView::OnCtlColorStatic(CDCHandle /*aDc*/, HWND /*aWindow*/)
{
  return (LRESULT)AtlGetStockBrush(WHITE_BRUSH);
}

LRESULT ActionsView::OnEraseBkgnd(CDCHandle aDc)
{
  CRect rc;
  GetClientRect(&rc);
  aDc.FillRect(&rc, AtlGetStockBrush(WHITE_BRUSH));
  return TRUE;
}

BOOL ActionsView::OnInitDialog(CWindow /*aWndFocus*/, LPARAM /*aInitParam*/)
{
  CButton but(GetDlgItem(IDC_BUT_OPEN));
  CRect rc;
  but.GetWindowRect(&rc);

  HBITMAP bmp = (HBITMAP)::LoadImage((HINSTANCE)::GetModuleHandle(NULL), 
    MAKEINTRESOURCE(IDB_BITMAP_OPEN), IMAGE_BITMAP, rc.Width(), rc.Height(), 0);
  but.SetBitmap(bmp);
  but.Detach();

  but.Attach(GetDlgItem(IDC_BUT_REMOVE));
  but.GetWindowRect(&rc);
  bmp = (HBITMAP)::LoadImage((HINSTANCE)::GetModuleHandle(NULL), 
    MAKEINTRESOURCE(IDB_BITMAP_REMOVE), IMAGE_BITMAP, rc.Width(), rc.Height(), 0);
  but.SetBitmap(bmp);

  mBackupInfoLink.SetHyperLinkExtendedStyle(HLINK_UNDERLINEHOVER | HLINK_COMMANDBUTTON);
  mBackupInfoLink.SubclassWindow(GetDlgItem(IDC_ST_BACKUP_INFO));

  DoFlushView(false);
  DlgResize_Init(false);

  return TRUE;
}

LRESULT ActionsView::OnBackup(WORD /*aNotifyCode*/, WORD /*aID*/, 
                              HWND /*aWindow*/, BOOL & /*aHandled*/)
{
  try
  {
    mUtBackup.Backup();
  }
  CATCH_EVERYTHING;

  return TRUE;
}

LRESULT ActionsView::OnRestore(WORD /*aNotifyCode*/, WORD /*aID*/, HWND /*aWindow*/, BOOL & /*aHandled*/)
{
  try
  {
    mUtBackup.Restore();
  }
  CATCH_EVERYTHING;
  
  return TRUE;
}

LRESULT ActionsView::OnOpen(WORD /*aNotifyCode*/, WORD /*aID*/, HWND /*aWindow*/, BOOL & /*aHandled*/)
{
  try
  {
    FilePath bkpFolder(mPropPool->GetProperty(UTB_BACKUPPATH_PROP));
    ::ShellExecute(NULL, L"explore", bkpFolder.GetParent().Get().c_str(), NULL, NULL, SW_SHOW);
  }
  CATCH_EVERYTHING;

  return TRUE;
}

LRESULT ActionsView::OnRemove(WORD /*aNotifyCode*/, WORD /*aID*/, HWND /*aWindow*/, BOOL & /*aHandled*/)
{
  try
  {
    FilePath path(mPropPool->GetProperty(UTB_BACKUPPATH_PROP));
    bEncodeManager bManager;
    File backupFile(path.Get());
    if (!backupFile.IsValid())
      throw Exception(*this, (path.Get() + L" was not found.").c_str());

    bManager.Decode(backupFile.Read());
    backupFile.Close();

    bool packed = bManager.GetItemValue(UTB_BACKUPPACKFILES_PROP) == PROP_TRUE;
    ::DeleteFile(path.Get().c_str());

    if (!packed)
    {
      FileUtil::RemoveDirectoryAndContents(path.GetParent().Get());
    }

    UIManager::Get().DisplayStatusMessage(L"Removed backup files");
  }
  CATCH_EVERYTHING

  return TRUE;
}

LRESULT ActionsView::OnBrowse(WORD /*aNotifyCode*/, WORD /*aID*/, HWND /*aWindow*/, BOOL & /*aHandled*/)
{
  try
  {
    CFileDialog browseDlg(TRUE, 0, 0, 6UL, L"uTorrent Backup Files\0*.utb\0\0", *this);

    if (browseDlg.DoModal() == IDOK)
    {
      FilePath backupPath(browseDlg.m_szFileName);
      backupPath.ReplaceExtension(UTB_DEF_FILE_EXTENSION);
      mPropPool->SetProperty(UTB_BACKUPPATH_PROP, backupPath.Get().c_str());
    }
  }
  CATCH_EVERYTHING;

  return TRUE;
}

LRESULT ActionsView::OnShowBackupInfo(WORD /*aNotifyCode*/, WORD /*aID*/, HWND /*aWindow*/, BOOL & /*aHandled*/)
{
  try
  {
    BackupInfoDlg bkpInfo(*mPropPool);

    bkpInfo.DoModal(*this);
  }
  CATCH_EVERYTHING;

  return TRUE;
}

void ActionsView::DoFlushView(bool aSave)
{
  if (aSave)
  {
    FilePath bkpPath(mBackupPath.GetString());
    mPropPool->SetProperty(UTB_BACKUPPATH_PROP, bkpPath.Get());
  }
  else
  {
    CEdit edit(GetDlgItem(IDC_EDIT_BACKUP_PATH));
    edit.SetWindowText(mPropPool->GetProperty(UTB_BACKUPPATH_PROP).c_str());
  }
}

//----------------------------------------------------------------------------
// IPropertyPool interface

void ActionsView::PropertyChanged(const wstring & aPropertyName, 
                                  const wstring & aPropertyValue)
{
  try
  {
    if (UTB_BACKUPPATH_PROP == aPropertyName)
    {
      CEdit edit(GetDlgItem(IDC_EDIT_BACKUP_PATH));
      edit.SetWindowText(aPropertyValue.c_str());
    }
  }
  CATCH_EVERYTHING;
}
