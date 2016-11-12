#pragma once

#include "settings/SettingsDialog.h"
#include "utbackup/UIManager.h"
#include "common/os/Registry.h"

class CMainFrame : 
	public CFrameWindowImpl<CMainFrame>, 
	public CUpdateUI<CMainFrame>,
	public CMessageFilter, public CIdleHandler
{
public:
  
  uTorrentBackup & mUtBackup;
	MainFrameView    m_view;
  
  CMainFrame(uTorrentBackup & aUtBackup) 
   : mUtBackup(aUtBackup), m_view(aUtBackup)
  {
  }
  DECLARE_FRAME_WND_CLASS(NULL, IDR_MAINFRAME)

  //--------------------------------------------------------------------------
	virtual BOOL PreTranslateMessage(MSG* pMsg)
	{
		if(CFrameWindowImpl<CMainFrame>::PreTranslateMessage(pMsg))
			return TRUE;

		return m_view.PreTranslateMessage(pMsg);
	}

	virtual BOOL OnIdle()
	{
		return FALSE;
	}

	BEGIN_UPDATE_UI_MAP(CMainFrame)
		UPDATE_ELEMENT(ID_VIEW_STATUS_BAR, UPDUI_MENUPOPUP)
	END_UPDATE_UI_MAP()

	BEGIN_MSG_MAP(CMainFrame)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
		COMMAND_ID_HANDLER(ID_APP_EXIT, OnFileExit)
		COMMAND_ID_HANDLER(ID_FILE_NEW, OnFileNew)
		COMMAND_ID_HANDLER(ID_VIEW_STATUS_BAR, OnViewStatusBar)
		COMMAND_ID_HANDLER(ID_APP_ABOUT, OnAppAbout)
    COMMAND_ID_HANDLER(ID_FILE_OPENROOTFOLDER, OnOpenRoot)
    COMMAND_ID_HANDLER(ID_FILE_SETTINGS, OnSettings)
    MSG_WM_SIZE(OnSize)
		CHAIN_MSG_MAP(CUpdateUI<CMainFrame>)
		CHAIN_MSG_MAP(CFrameWindowImpl<CMainFrame>)    

    //REFLECT_NOTIFICATIONS()
	END_MSG_MAP()
  
// Handler prototypes (uncomment arguments if needed):
//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

	LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{

		CreateSimpleStatusBar();

		m_hWndClient = m_view.Create(m_hWnd);
		UISetCheck(ID_VIEW_STATUS_BAR, 1); 
        
		// register object for message filtering and idle updates
		CMessageLoop* pLoop = _Module.GetMessageLoop();
		ATLASSERT(pLoop != NULL);
		pLoop->AddMessageFilter(this);
		pLoop->AddIdleHandler(this);

		return 0;
	}

	LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
	{
		// unregister message filtering and idle updates
		CMessageLoop* pLoop = _Module.GetMessageLoop();
		ATLASSERT(pLoop != NULL);
		pLoop->RemoveMessageFilter(this);
		pLoop->RemoveIdleHandler(this);

    RECT rc;
    GetWindowRect(&rc);
    Registry reg(HKEY_CURRENT_USER, Globals::kUtbRegSettingsKey, true);
    reg.WriteBinary(L"Window Rect", reinterpret_cast<const byte *>(&rc), sizeof(RECT));

		bHandled = FALSE;
		return 1;
	}

	LRESULT OnFileExit(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		PostMessage(WM_CLOSE);
		return 0;
	}

  LRESULT OnOpenRoot(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
  {
    FilePath utPath(mUtBackup.GetPropertyPool().GetProperty(UT_PATH_PROP));

    ::ShellExecute(NULL, L"open", utPath.Get().c_str(), NULL, NULL, SW_SHOWNORMAL);

    return 0;
  }

  LRESULT OnSettings(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
  {
    try
    {
      SettingsDlg dlg(mUtBackup.GetPropertyPool());

      dlg.DoModal();
    }
    CATCH_EVERYTHING

    return 0;
  }

	LRESULT OnFileNew(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		// TODO: add code to initialize document

		return 0;
	}

	LRESULT OnViewStatusBar(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		BOOL bVisible = !::IsWindowVisible(m_hWndStatusBar);
		::ShowWindow(m_hWndStatusBar, bVisible ? SW_SHOWNOACTIVATE : SW_HIDE);
		UISetCheck(ID_VIEW_STATUS_BAR, bVisible);
		UpdateLayout();
		return 0;
	}

	LRESULT OnAppAbout(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		CAboutDlg dlg;
		dlg.DoModal();
		return 0;
	}

  void OnSize(UINT /*aType*/, CSize /*aSize*/)
  {
    CRect rc;
    GetClientRect(&rc);
    m_view.SetWindowPos(NULL, &rc, SWP_SHOWWINDOW);

    /**
     * Notify UIManager to update it's layout.
     */
    UIManager::Get().NotifySizeChanged();

    SetMsgHandled(FALSE);
  }
};
