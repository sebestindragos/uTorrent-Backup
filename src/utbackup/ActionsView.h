#ifndef _ACTIONS_VIEW_H_
#define _ACTIONS_VIEW_H_

#ifndef _STDAFX_INCLUDED_
  #error "Must include stdafx first."
#endif

#include "utbackup/propertypool/IPropertyObserver.h"

class uTorrentBackup;
class IPropertyPool;

/**
 * Class implementing the actions tab view.
 *
 * @author Sebestin Dragos
 */
class ActionsView : public CDialogImpl<ActionsView>,
                    public CDialogResize<ActionsView>,
                    public IPropertyObserver
{
public:
  
  enum { IDD = 101};

  ActionsView(HWND aParent, uTorrentBackup & aPropPool);

  BEGIN_MSG_MAP(ActionsView)
    MSG_WM_ERASEBKGND(OnEraseBkgnd)
    MSG_WM_CTLCOLORSTATIC(OnCtlColorStatic)
    MSG_WM_INITDIALOG(OnInitDialog)
    COMMAND_ID_HANDLER(IDC_BUT_BACKUP, OnBackup)
    COMMAND_ID_HANDLER(IDC_BUT_RESTORE, OnRestore)
    COMMAND_ID_HANDLER(IDC_BUT_OPEN, OnOpen)
    COMMAND_ID_HANDLER(IDC_BUT_REMOVE, OnRemove)
    COMMAND_ID_HANDLER(IDC_BUT_BROWSE, OnBrowse)
    COMMAND_ID_HANDLER(IDC_ST_BACKUP_INFO, OnShowBackupInfo)

    CHAIN_MSG_MAP(CDialogResize<ActionsView>)
  END_MSG_MAP()
    
  BEGIN_DLGRESIZE_MAP(ActionsView)
    DLGRESIZE_CONTROL(IDC_EDIT_BACKUP_PATH, DLSZ_SIZE_X)
    DLGRESIZE_CONTROL(IDC_BUT_BROWSE, DLSZ_MOVE_X)
    DLGRESIZE_CONTROL(IDC_BUT_RESTORE, DLSZ_MOVE_X)
  END_DLGRESIZE_MAP()
  
private:

  CString          mBackupPath;
  uTorrentBackup & mUtBackup;
  const CString    kAlternateBackupFolder;
  IPropertyPool  * mPropPool;
  CHyperLink       mBackupInfoLink;
  
  //--------------------------------------------------------------------------
  // IPropertyPool interface

  void PropertyChanged(const wstring & aPropertyName, 
    const wstring & aPropertyValue);

  //--------------------------------------------------------------------------

  ActionsView();

  LRESULT OnEraseBkgnd(CDCHandle aDc);
  LRESULT OnCtlColorStatic(CDCHandle aDc, HWND aWindow);
  void DoFlushView(bool aSave);

  BOOL OnInitDialog(CWindow aWndFocus, LPARAM aInitParam);

  LRESULT OnBackup(WORD aNotifyCode, WORD aID, HWND aWindow, BOOL & aHandled);

  LRESULT OnRestore(WORD aNotifyCode, WORD aID, HWND aWindow, BOOL & aHandled);

  LRESULT OnOpen(WORD aNotifyCode, WORD aID, HWND aWindow, BOOL & aHandled);

  LRESULT OnRemove(WORD aNotifyCode, WORD aID, HWND aWindow, BOOL & aHandled);

  LRESULT OnBrowse(WORD aNotifyCode, WORD aID, HWND aWindow, BOOL & aHandled);

  LRESULT OnShowBackupInfo(WORD aNotifyCode, WORD aID, HWND aWindow, BOOL & aHandled);
};

#endif // _ACTIONS_VIEW_H_
