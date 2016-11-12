#ifndef _WIN_TASKBAR_PROGRESS_H_
#define _WIN_TASKBAR_PROGRESS_H_

/**
 * Class used for manipulating the windows 7 and above taskbar.
 *
 * @author : Dragos Sebestin
 */
class WinTaskbar
{
public:

  enum TaskbarProgressState {NoProgress = 0, Indeterminate = 1, Normal = 2,
                             Error = 4, Paused = 8};
  
  /**
   * Set the progress state of the taskbar progress.
   */
  bool SetProgressState(HWND aWindow, TaskbarProgressState aState);

  /**
   * Set the progress value of the taskbar progress.
   */
  bool SetProgressValue(HWND aWindow, ULONGLONG aCompleted, ULONGLONG aTotal);

  /**
   * Set the taskbar thumbnail of the window to a certain region
   */
  bool SetThumbnailClip(HWND aWindow, LPRECT aRect);

  /**
   * Add buttons to the thumbnail.
   */
  bool AddButtons(HWND aWindow, const vector<THUMBBUTTON> & aButtons);

  /**
   * Add a button to the internal container.
   */
  bool AddButton(UINT aId, HICON aResourceID, const wstring & aToolTip);

  /**
   * Add stored buttons to a window's taskbar.
   */
  bool CommitButtons(HWND aWindow);

  /**
   * Get the singleton object.
   */
  static WinTaskbar & Get();

private:

  WinTaskbar();
  ~WinTaskbar();

  vector<THUMBBUTTON> mButtons;

  ITaskbarList3 * mTaskbarList;
};

#endif // _WIN_TASKBAR_PROGRESS_H_
