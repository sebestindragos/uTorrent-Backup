#include "stdafx.h"
#include "AdvancedView.h"
#include "propertypool/IPropertyPool.h"
#include "bEncode/bEncodeManager.h"
#include "common/util/FileUtil.h"
#include "common/os/WinTaskbar.h"
#include "utbackup/utorrentbackup.h"

AdvancedView::AdvancedView(HWND aParent, uTorrentBackup & aUtBackup)
 : mPropertyPool(aUtBackup.GetPropertyPool()), mUtBackup(aUtBackup),
   mCleanupList(mPropertyPool)
{
  CDialogImpl::Create(aParent);
}

LRESULT AdvancedView::OnCtlColorStatic(CDCHandle /*aDc*/, HWND /*aWindow*/)
{
  return (LRESULT)AtlGetStockBrush(WHITE_BRUSH);
}

LRESULT AdvancedView::OnEraseBkgnd(CDCHandle aDc)
{
  CRect rc;
  GetClientRect(&rc);
  aDc.FillRect(&rc, AtlGetStockBrush(WHITE_BRUSH));
  return TRUE;
}

BOOL AdvancedView::OnInitDialog(CWindow /*aWndFocus*/, LPARAM /*aInitParam*/)
{
  try
  {
    mCleanupList.Attach(GetDlgItem(IDC_LIST_CLEAN_FILES));

    DlgResize_Init(false);
  }
  CATCH_EVERYTHING;
  
  return TRUE;
}


LRESULT AdvancedView::OnScan(WORD /*aNotifyCode*/, WORD /*aID*/, 
                             HWND /*aWndCtl*/, BOOL& /*aHandled*/)
{
  mCleanupList.ScanFiles();
  return 0;
}

LRESULT AdvancedView::OnClean(WORD /*aNotifyCode*/, WORD /*aID*/, 
                              HWND /*aWndCtl*/, BOOL& /*aHandled*/)
{
  mCleanupList.CleanFiles();
  return 0;
}

void AdvancedView::DlgResize_UpdateLayout(int cxWidth, int cyHeight)
{
  try
  {
    CRect clientRc;
    mCleanupList.GetClientRect(&clientRc);

    // resize the last column
    mCleanupList.SetColumnWidth(1, clientRc.Width() - 
      mCleanupList.GetColumnWidth(0) - mCleanupList.GetColumnWidth(2));
  }
  CATCH_EVERYTHING

  CDialogResize::DlgResize_UpdateLayout(cxWidth, cyHeight);
}
