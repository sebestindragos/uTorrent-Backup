#include "stdafx.h"
#include "UIUtil.h"

//----------------------------------------------------------------------------
void UIUtil::SetListViewExplorerTheme(HWND aListWnd)
{
  static HMODULE hUxTheme(::LoadLibrary(L"UxTheme.dll"));
  static HRESULT (WINAPI *pSetWindowTheme)(HWND, LPCWSTR, LPCWSTR) =
    (HRESULT (WINAPI *)(HWND, LPCWSTR, LPCWSTR))::GetProcAddress(hUxTheme, "SetWindowTheme");

  if( pSetWindowTheme != NULL )
  {
    pSetWindowTheme(aListWnd, L"explorer", NULL);
    CListViewCtrl ctrl(aListWnd);
    ctrl.SetExtendedListViewStyle(LVS_EX_DOUBLEBUFFER, LVS_EX_DOUBLEBUFFER);
  }
}

void UIUtil::ListViewAddSimpleColumn(HWND aList, const CString & aText, int aWidth)
{
  CListViewCtrl listCtrl(aList);
  CHeaderCtrl header(listCtrl.GetHeader());

  LVCOLUMN lvCol = {sizeof(LVCOLUMN)};
  lvCol.mask = LVCF_TEXT | LVCF_WIDTH;
  lvCol.pszText = const_cast<LPWSTR>(aText.GetString());
  lvCol.cchTextMax = aText.GetLength();
  lvCol.cx = aWidth;
  
  listCtrl.InsertColumn(header.GetItemCount(), &lvCol);
}
