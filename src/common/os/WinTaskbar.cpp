#include "stdafx.h"
#include "WinTaskbar.h"
#include "WinVersion.h"


WinTaskbar::WinTaskbar() : mTaskbarList(nullptr)
{
  if (WinVer::IsWindows7())
  {
    ::CoInitialize(NULL);
    ::CoCreateInstance(CLSID_TaskbarList, NULL, CLSCTX_INPROC_SERVER,
                       IID_ITaskbarList3, (void **) &mTaskbarList);  
  }
}

WinTaskbar::~WinTaskbar()
{
}

bool WinTaskbar::SetProgressState(HWND aWindow, 
                                  TaskbarProgressState aState)
{
  if (!mTaskbarList)
    return false;
  return S_OK == mTaskbarList->SetProgressState(aWindow, (TBPFLAG) aState);
}

bool WinTaskbar::SetProgressValue(HWND aWindow, ULONGLONG aCompleted, 
                                  ULONGLONG aTotal)
{
  if (!mTaskbarList)
    return false;
  return S_OK == mTaskbarList->SetProgressValue(aWindow, aCompleted, aTotal);
}

bool WinTaskbar::SetThumbnailClip(HWND aWindow, LPRECT aRect)
{
  if (!mTaskbarList)
    return false;
  return S_OK == mTaskbarList->SetThumbnailClip(aWindow, aRect);
}

bool WinTaskbar::AddButtons(HWND aWindow, const vector<THUMBBUTTON> & aButtons)
{
  if (!mTaskbarList || aButtons.size() == 0)
    return false;
  return S_OK == 
    mTaskbarList->ThumbBarAddButtons(aWindow, (UINT)aButtons.size(), const_cast<LPTHUMBBUTTON>(&aButtons[0]));
}

bool WinTaskbar::AddButton(UINT aId, HICON aResourceId, const wstring & aTooltip)
{
  if (!mTaskbarList)
    return false;
  
  THUMBBUTTON crtButton;

  crtButton.iId = aId;
  crtButton.dwFlags = THBF_ENABLED;
  crtButton.dwMask = THB_ICON | THB_FLAGS;
  crtButton.hIcon = aResourceId;

  if (aTooltip.size() > 0)
  {
    crtButton.dwMask |= THB_TOOLTIP;
    wcscpy_s(crtButton.szTip, aTooltip.size() + 1, aTooltip.c_str());
  }

  mButtons.push_back(crtButton);
  return true;
}

bool WinTaskbar::CommitButtons(HWND aWindow)
{
  if (!mTaskbarList)
    return false;

  return S_OK == 
    mTaskbarList->ThumbBarAddButtons(aWindow, (UINT)mButtons.size(), &mButtons[0]);
}

WinTaskbar & WinTaskbar::Get()
{
  static WinTaskbar winTaskbar;

  return winTaskbar;
}
