#pragma once

#ifndef _STDAFX_INCLUDED_
//  #error "Must include stdafx first."
#endif

class uTorrentBackup;
typedef unordered_map<wstring, CWindow *> ViewMap;

/**
 * Main view of the application
 *
 * @author: Dragos Sebestin
 */
class MainFrameView : public CDialogImpl<MainFrameView>,
                           public CDialogResize<MainFrameView>
{
public:
	enum { IDD = IDD_UTORRENTBACKUP_FORM };

  MainFrameView(uTorrentBackup & aUtBackup);
    
	BEGIN_MSG_MAP(MainFrameView)
    MSG_WM_INITDIALOG(OnInitDialog)
    NOTIFY_CODE_HANDLER(TCN_SELCHANGE, OnTabSelChange)

    CHAIN_MSG_MAP(CDialogResize<MainFrameView>)
	END_MSG_MAP()

  BEGIN_DLGRESIZE_MAP(MainFrameView)
    DLGRESIZE_CONTROL(IDC_TABCTRL_VIEWS, DLSZ_SIZE_X | DLSZ_SIZE_Y)
  END_DLGRESIZE_MAP()
//----------------------------------------------------------------------------

  BOOL PreTranslateMessage(MSG* pMsg);
  BOOL OnInitDialog(CWindow aWndFocus, LPARAM aInitParam);
  void DlgResize_UpdateLayout(int cxWidth, int cyHeight);
  
  LRESULT OnTabSelChange(int aId, LPNMHDR aNMHDR, BOOL & aHandled);

private:
  CTabCtrl         mViewsTab;
  CWindow        * mActiveView;
  ViewMap          mViewMap;
  uTorrentBackup & mUtBackup;

// Handler prototypes (uncomment arguments if needed):
//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)
};
