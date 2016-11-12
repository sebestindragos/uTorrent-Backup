#include "stdafx.h"
#include "MainFrameView.h"
#include "uTorrentBackup.h"
#include "ActionsView.h"
#include "AdvancedView.h"
#include "bencode/bEncodeManager.h"
#include "common/util/FileUtil.h"
#include "common/os/WinTaskbar.h"


#define ACTIONS_TAB                  L"Actions"
#define ADVANCED_TAB                 L"Advanced"

//----------------------------------------------------------------------------
MainFrameView::MainFrameView(uTorrentBackup & aUtBackup)
  : mActiveView(nullptr), mUtBackup(aUtBackup)
{
}

BOOL MainFrameView::PreTranslateMessage(MSG* pMsg)
{
  return CWindow::IsDialogMessage(pMsg);
}

BOOL MainFrameView::OnInitDialog(CWindow /*aWndFocus*/, LPARAM /*aInitParam*/)
{
  // add tab items
  mViewsTab.Attach(GetDlgItem(IDC_TABCTRL_VIEWS));
  mViewsTab.AddItem(ACTIONS_TAB);
  mViewsTab.AddItem(ADVANCED_TAB);

  // create views
  mViewMap[ACTIONS_TAB] = new ActionsView(mViewsTab, mUtBackup);
  mViewMap[ADVANCED_TAB] = new AdvancedView(mViewsTab, mUtBackup);

  mViewMap[ADVANCED_TAB]->ShowWindow(false);
  
  mActiveView = mViewMap[ACTIONS_TAB];
 
  DlgResize_Init(false);

  return TRUE;
}

void MainFrameView::DlgResize_UpdateLayout(int cxWidth, int cyHeight)
{
  CDialogResize::DlgResize_UpdateLayout(cxWidth, cyHeight);
  CRect rc;
  mViewsTab.GetClientRect(&rc);
  //mViewsTab.ClientToScreen(rc);
  mViewsTab.AdjustRect(FALSE, &rc);

  auto it = mViewMap.begin();
  auto end = mViewMap.end();
  for (; it != end; ++it)
  {
    CWindow wnd = *(it->second);
    wnd.MoveWindow(&rc);
    wnd.Invalidate();
  }

  
}

LRESULT MainFrameView::OnTabSelChange(int /*aId*/, LPNMHDR /*aNMHDR*/, BOOL & aHandled)
{
  CString selItemText;
  
  TCITEM tcit;
  tcit.mask = TCIF_TEXT;
  tcit.pszText = selItemText.GetBuffer(512);
  tcit.cchTextMax = 512;
  mViewsTab.GetItem(mViewsTab.GetCurSel(), &tcit);
  selItemText.ReleaseBuffer();
  
  // flush old view and hide it
  CWindow * oldView = mActiveView;
  //oldView->SendMessage(WM_FLUSH, 0, 0);
  
  mActiveView = mViewMap[selItemText.GetString()];

  if (oldView)
    oldView->ShowWindow(0);
  if (mActiveView)
    mActiveView->ShowWindow(SW_SHOW);
  else
    return FALSE;
  
  CRect rc;
  GetWindowRect(&rc);
  DlgResize_UpdateLayout(rc.right - rc.left, rc.bottom - rc.top);

  aHandled = FALSE;
  return FALSE;
}
