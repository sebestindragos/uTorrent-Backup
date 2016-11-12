#ifndef _SETTINGS_DIALOG_H_
#define _SETTINGS_DIALOG_H_

#ifndef _STDAFX_INCLUDED_
#error "Must include stdafx first."
#endif

class IPropertyPool;

/**
 * Class implementing the settings dialog
 *
 * @author Dragos Sebestin
 */
class SettingsDlg : public CDialogImpl<SettingsDlg>,
                    public CWinDataExchange<SettingsDlg>
{
public:

  enum {IDD = IDD_EDIT_SETTINGS};

  BEGIN_MSG_MAP(SettingsDlg)
    COMMAND_ID_HANDLER(IDOK, OnOk)
    COMMAND_ID_HANDLER(IDCANCEL, OnCancel)
    MSG_WM_INITDIALOG(OnInitDialog)
  END_MSG_MAP()

  SettingsDlg(IPropertyPool & aPropPool);

private:

  IPropertyPool & mPropPool;
  CListBox mList;

  LRESULT OnOk(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
  LRESULT OnCancel(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);

  LRESULT OnInitDialog(HWND aWindow, LPARAM aParam);

  void PopulateList();

  void DoDataExchange(bool aSaveAndValidate);
};

#endif // _SETTINGS_DIALOG_H_