#ifndef _UIUTIL_H_
#define _UIUTIL_H_

/**
 * Various helper methods for working with UI controls.
 *
 * @author Dragos Sebestin
 */
namespace UIUtil
{
  void SetListViewExplorerTheme(HWND aListWnd);
  void ListViewAddSimpleColumn(HWND aList, const CString & aText, int aWidth = 100);
}

#endif // _UIUTIL_H_
