#include "stdafx.h"
#include "uTorrentBackup.h"
#include "MainFrameView.h"
#include "aboutdlg.h"
#include "MainFrm.h"
#include "UIManager.h"

CAppModule _Module;

int Run(LPTSTR lpstrCmdLine = NULL, int nCmdShow = SW_SHOWDEFAULT)
{
  uTorrentBackup utBackup;

  try
  {
    // if a command line was passed, it contains a path to a utb file. Use it
    // to do a restore operation.
    if (lpstrCmdLine && *lpstrCmdLine)
    {
      wstring filePath = StringUtil::StripQuotes(lpstrCmdLine);
      utBackup.GetPropertyPool().SetProperty(UTB_BACKUPPATH_PROP, filePath);
      utBackup.Restore(true); // wait for the restore operation to finish.
      if (utBackup.HasException())
        utBackup.RaiseException();
      UIManager::Get().DisplayInfoMessage(L"Restore completed successfully.");

      return 0;
    }
  }
  CATCH_EVERYTHING;


  CMessageLoop theLoop;
  _Module.AddMessageLoop(&theLoop);
	
  // init main application window
  CMainFrame wndMain(utBackup);
  
  RECT rc = {0, 0, 580, 280};
  Registry reg(HKEY_CURRENT_USER, Globals::kUtbRegSettingsKey, true);
  if (reg.KeyExists())
    reg.ReadBinary(L"Window Rect", reinterpret_cast<byte *>(&rc));
  if(wndMain.CreateEx(NULL, rc) == NULL)
	{
		ATLTRACE(_T("Main window creation failed!\n"));
		return 0;
	}

  // initialize UIManager singleton
  UIManager::Get().Attach(wndMain);
  
  wndMain.CenterWindow();
  wndMain.ShowWindow(nCmdShow);

//   wndMain.GetClientRect(&rc);
//   InflateRect(&rc, -50, -50);
// 
//   WinTaskbarProgress tbp;
//   //tbp.SetThumbnailClip(wndMain, &rc);
// 
//   tbp.AddButton(IDC_BUT_OPEN, IDR_MAINFRAME, L"Open the backup folder");
//   tbp.CommitButtons(wndMain);
// 
//   tbp.SetProgressValue(wndMain, 70, 100);
//   tbp.SetProgressState(wndMain, WinTaskbarProgress::Error);

	int nRet = theLoop.Run();

	_Module.RemoveMessageLoop();
	return nRet;
}

int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPTSTR lpstrCmdLine, int nCmdShow)
{
	HRESULT hRes = ::CoInitialize(NULL);
// If you are running on NT 4.0 or higher you can use the following call instead to 
// make the EXE free threaded. This means that calls come in on a random RPC thread.
//	HRESULT hRes = ::CoInitializeEx(NULL, COINIT_MULTITHREADED);
	ATLASSERT(SUCCEEDED(hRes));

	// this resolves ATL window thunking problem when Microsoft Layer for Unicode (MSLU) is used
	::DefWindowProc(NULL, 0, 0, 0L);

	AtlInitCommonControls(ICC_BAR_CLASSES | ICC_LISTVIEW_CLASSES);	// add flags to support other controls

	hRes = _Module.Init(NULL, hInstance);
	ATLASSERT(SUCCEEDED(hRes));

  int nRet = Run(lpstrCmdLine, nCmdShow);

	_Module.Term();
	::CoUninitialize();

	return nRet;
}
