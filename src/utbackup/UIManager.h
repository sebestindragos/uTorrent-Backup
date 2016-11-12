#ifndef _UI_MANAGER_H_
#define _UI_MANAGER_H_

#ifndef _STDAFX_INCLUDED_
  #error "Must include stdafx first."
#endif

#include "common/util/SyncGuard.h"

/**
 * Class used for UI presentation.
 *
 * @author: Dragos Sebestin
 */
class UIManager
{
public:

  enum UILevel {None, Full};

  /**
   * Get singleton instance.
   */
  static UIManager & Get();

  /**
   * Attach to a window.
   */
  void Attach(HWND aWindow);

  /**
   * Get the attached window.
   */
  HWND GetWindow();

  /**
   * Detach from window.
   */
  void Detach();

  /**
   * Set status bar text.
   */
  void DisplayStatusMessage(const CString & aMessage);

  /**
   * Display a message inside a messagebox with no parrent window
   */
  void DisplayInfoMessage(const CString & aMessage);
  
  /**
   * Set the progress interval.
   */
  void SetProgress(int aCurrent, int aTotal);

  /**
   * Handle main window size changes.
   */
  void NotifySizeChanged();

private:
    
  HWND mHwnd;
  HWND mStatusBar;
  CProgressBarCtrl mProgressbar;
  SyncSupport mThreadLock;
  UILevel mUILevel;
  
  /**
   * Class constructors.
   */
  UIManager();

  void Init();
};

#endif // _UI_MANAGER_H_

