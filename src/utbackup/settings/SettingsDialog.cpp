#include "stdafx.h"
#include "SettingsDialog.h"
#include "propertypool/IPropertyPool.h"

struct Page
{
  wstring mName;
};

Page kPages [] =
{
  L"General"
};

SettingsDlg::SettingsDlg(IPropertyPool & aPropPool)
 : mPropPool(aPropPool) 
{
}

LRESULT SettingsDlg::OnOk(WORD /*wNotifyCode*/, WORD wID, 
                          HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
  try
  {
    DoDataExchange(true);
    EndDialog(wID);
  }
  CATCH_EVERYTHING;

  return 0;
}

LRESULT SettingsDlg::OnCancel(WORD /*wNotifyCode*/, WORD wID, 
  HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
  try
  {
    EndDialog(wID);
  }
  CATCH_EVERYTHING;

  return 0;
}

LRESULT SettingsDlg::OnInitDialog(HWND /*aWindow*/, LPARAM /*aParam*/)
{
  try
  {
    CStatic header(GetDlgItem(IDC_HEADER));
    header.SetWindowText(kPages[0].mName.c_str());

    mList.Attach(GetDlgItem(IDC_LIST_SETTINGS));

    CButton btn(GetDlgItem(IDC_CHECK_BACKUP_ALTERNATE));
    if (mPropPool.GetProperty(UT_ALTERNATEFOLDER_PROP) == PROP_TRUE)
      btn.EnableWindow(true);

    PopulateList();
    DoDataExchange(false);
  }
  CATCH_EVERYTHING;

  return 0;
}

void SettingsDlg::PopulateList()
{
  size_t count = sizeof(kPages) / sizeof(kPages[0]);

  for (size_t i = 0;  i < count; i++)
  {
    mList.AddString(kPages[i].mName.c_str());
  }

  mList.SetCurSel(0);
}

void SettingsDlg::DoDataExchange(bool aSaveAndValidate)
{
  int packFiles = 0;
  int backupAlternate = 0;

  if (!aSaveAndValidate)
  {
    wstring prop = mPropPool.GetProperty(UTB_BACKUPPACKFILES_PROP).c_str();
    packFiles = (prop == PROP_TRUE ? 1 : 0);

    prop = mPropPool.GetProperty(UTB_BACKUPALTERNATE_PROP).c_str();
    backupAlternate = (prop == PROP_TRUE ? 1 : 0);
  }

  DDX_Check(IDC_CHECK_PACKFILES, packFiles, aSaveAndValidate);
  DDX_Check(IDC_CHECK_BACKUP_ALTERNATE, backupAlternate, aSaveAndValidate);

  if (aSaveAndValidate)
  {
    wstring propVal = packFiles == 1 ? PROP_TRUE : PROP_FALSE;
    mPropPool.SetProperty(UTB_BACKUPPACKFILES_PROP, propVal);
    propVal = backupAlternate == 1 ? PROP_TRUE : PROP_FALSE;
    mPropPool.SetProperty(UTB_BACKUPALTERNATE_PROP, propVal);
  }
}
