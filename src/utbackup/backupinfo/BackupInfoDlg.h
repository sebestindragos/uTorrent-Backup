#ifndef _BACKUP_INFO_DLG_H_
#define _BACKUP_INFO_DLG_H_

#ifndef _STDAFX_INCLUDED_
  #error "Must include stdafx first."
#endif

#include "resource.h"
#include "bencode/bEncodeManager.h"

class IPropertyPool;

/**
 * Class used for displaying the backup info dialog.
 *
 * @author Dragos Sebestin
 */
class BackupInfoDlg : public CDialogImpl<BackupInfoDlg>,
                      public CDialogResize<BackupInfoDlg>
{
public:

  enum { IDD = IDD_BACKUP_INFO };

  BEGIN_MSG_MAP(BackupInfoDlg)
    MSG_WM_INITDIALOG(OnInitDialog)
    COMMAND_ID_HANDLER(IDOK, OnOk)
    COMMAND_ID_HANDLER(IDCANCEL, OnOk)
    MSG_WM_GETDLGCODE(OnKeyDown)
    
    CHAIN_MSG_MAP(CDialogResize<BackupInfoDlg>)
  END_MSG_MAP()

  BEGIN_DLGRESIZE_MAP(BackupInfoDlg)
    DLGRESIZE_CONTROL(IDC_GRP_BACKUP_INFO, DLSZ_SIZE_X | DLSZ_SIZE_Y)
    DLGRESIZE_CONTROL(IDC_EDIT_FILTER, DLSZ_SIZE_X)
    DLGRESIZE_CONTROL(IDC_TREE_INFO, DLSZ_SIZE_X | DLSZ_SIZE_Y)
    DLGRESIZE_CONTROL(IDOK, DLSZ_MOVE_X | DLSZ_MOVE_Y)
  END_DLGRESIZE_MAP()

  // class c-tor
  BackupInfoDlg(IPropertyPool & aPropPool);
  
  BOOL PreTranslateMessage(MSG* pMsg)
  {
    pMsg;
//     if (pMsg->message == WM_KEYDOWN &&
//       //pMsg->wParam == VK_RETURN &&
//       GetFocus() == mFilterEdit)
//     {
//       // handle return pressed in edit control
//       return TRUE; // this doesn't need processing anymore
//     }

    return FALSE;
  }


private:

  IPropertyPool & mPropPool;
  CTreeViewCtrl   mTree;
  CComboBox       mFilterCombo;
  CEdit           mFilterEdit;
  bEncodeManager  mbManager;

  enum FilterType {Label, Name};

  BOOL OnInitDialog(CWindow aWndFocus, LPARAM aInitParam);
  LRESULT OnOk(WORD aNotifyCode, WORD aID, HWND aWindow, BOOL & aHandled);
  LRESULT OnKeyDown(MSG *)
  {
    return 0;
  }
  
  void PopulateFilterCombo();
  void DecodeProjectFile();
  void PopulateInfoTree();

  // check if an item matches the filter
  bool MatchesFilter(bEncodeItem * aItem, FilterType aFilterType, const wstring & aFilterText);
};

#endif // _BACKUP_INFO_DLG_H_
