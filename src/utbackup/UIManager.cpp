#include "stdafx.h"
#include "UIManager.h"
#include "common/os/WinVersion.h"
#include "common/os/WinTaskbar.h"

//----------------------------------------------------------------------------
// Class definition - public members.
UIManager::UIManager() 
 : mHwnd(NULL), mStatusBar(NULL), mUILevel(None)
{
}

void UIManager::DisplayStatusMessage(const CString & aMessage)
{
  SyncGuard sg(mThreadLock);

  ATLASSERT(::IsWindow(mStatusBar));
  ::SetWindowText(mStatusBar, aMessage);
}

void UIManager::Attach(HWND aWindow)
{
  mHwnd = aWindow;
  Init();
}

void UIManager::Detach()
{
  mHwnd = NULL;
  mStatusBar = NULL;
  mProgressbar.DestroyWindow();
}

void UIManager::Init()
{
  if (mHwnd)
    mUILevel = Full;

  // Get the window status bar
  mStatusBar = ::GetDlgItem(mHwnd, ATL_IDW_STATUS_BAR);
  
  // on pre win7 OSes create a progress bar in the status bar.
  if (!WinVer::IsWindows7() && mStatusBar)
  {
    RECT rc;
    mProgressbar.Create(mStatusBar, rc, 0, WS_CHILD | WS_VISIBLE);
  }
}

UIManager & UIManager::Get()
{
  static UIManager mUIManager;

  return mUIManager;
}

void UIManager::DisplayInfoMessage(const CString & aMessage)
{
  ::MessageBox(NULL, aMessage, L"Information", MB_ICONINFORMATION | MB_OK);
}

HWND UIManager::GetWindow()
{
  return mHwnd;
}

void UIManager::SetProgress(int aCurrent, int aTotal)
{
  if (WinVer::IsWindows7())
  {
    WinTaskbar::Get().SetProgressState(mHwnd, WinTaskbar::Normal);
    WinTaskbar::Get().SetProgressValue(mHwnd, aCurrent, aTotal);
  }
  else
  {
    mProgressbar.SetRange(0, aTotal);
    mProgressbar.SetPos(aCurrent);
  }
}

void UIManager::NotifySizeChanged()
{
  if (mProgressbar.IsWindow())
  {
    CRect rc;
    ::GetClientRect(mStatusBar, &rc);
    rc.left = rc.right - 180; rc.right -= 30;
    rc.top += 3; rc.bottom -= 3;

    mProgressbar.MoveWindow(&rc);
  }
}
