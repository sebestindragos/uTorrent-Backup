#ifndef _IPROPERTY_POOL_H_
#define _IPROPERTY_POOL_H_

#ifndef _STDAFX_INCLUDED_
  #error "Must include stdafx first."
#endif

#include "CleanupList.h"

class IPropertyPool;
class uTorrentBackup;

/**
 * Class implementing the advanced tab view
 *
 * @author: Dragos Sebestin
 */
class AdvancedView : public CDialogImpl<AdvancedView>,
                     public CDialogResize<AdvancedView>
{
public:
    
  enum {IDD = IDD_ADVANCED_VIEW};

  AdvancedView(HWND aParent, uTorrentBackup & aUtBackup);

  BEGIN_MSG_MAP(AdvancedView)
    MSG_WM_INITDIALOG(OnInitDialog)
    MSG_WM_CTLCOLORSTATIC(OnCtlColorStatic)
    MSG_WM_ERASEBKGND(OnEraseBkgnd)
    COMMAND_ID_HANDLER(IDC_BUT_SCAN, OnScan)
    COMMAND_ID_HANDLER(IDC_BUT_CLEAN, OnClean)

    CHAIN_MSG_MAP(CDialogResize<AdvancedView>)
    REFLECT_NOTIFICATIONS()
  END_MSG_MAP()

  BEGIN_DLGRESIZE_MAP(AdvancedView)
    DLGRESIZE_CONTROL(IDC_GRP_MAINT, DLSZ_SIZE_X | DLSZ_SIZE_Y)
    DLGRESIZE_CONTROL(IDC_ST_CLEANUP_INFO, DLSZ_SIZE_X)
    DLGRESIZE_CONTROL(IDC_LIST_CLEAN_FILES, DLSZ_SIZE_X | DLSZ_SIZE_Y)
    DLGRESIZE_CONTROL(IDC_BUT_SCAN, DLSZ_MOVE_Y)
    DLGRESIZE_CONTROL(IDC_BUT_CLEAN, DLSZ_MOVE_X | DLSZ_MOVE_Y)
  END_DLGRESIZE_MAP()

  void DlgResize_UpdateLayout(int cxWidth, int cyHeight);
  
private:

  uTorrentBackup & mUtBackup;
  IPropertyPool  & mPropertyPool;
  CleanupList      mCleanupList;

  LRESULT OnEraseBkgnd(CDCHandle aDc);

  LRESULT OnCtlColorStatic(CDCHandle aDc, HWND aWindow);

  BOOL OnInitDialog(CWindow aWndFocus, LPARAM aInitParam);

  LRESULT OnScan(WORD aNotifyCode, WORD aID, HWND aWndCtl, BOOL& aHandled);
  LRESULT OnClean(WORD aNotifyCode, WORD aID, HWND aWndCtl, BOOL& aHandled);
};

#endif // _IPROPERTY_POOL_H_
