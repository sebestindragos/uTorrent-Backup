#ifndef _CLEANUP_LIST_H_
#define _CLEANUP_LIST_H_

#ifndef _STDAFX_INCLUDED_
  #error "Must include stdafx first."
#endif

#include "common/util/IWorkerThreadObserver.h"

class IPropertyPool;
class UIManager;
class WorkerThread;

/**
 * Class used for displaying the list of leftover files.
 *
 * @author Dragos Sebestin
 */
class CleanupList : public CWindowImpl<CleanupList, CListViewCtrl>,
                    public IWorkerThreadObserver
{
public:

  /**
   * Class constructor.
   */
  CleanupList(IPropertyPool & aPropPool);
  ~CleanupList();

  BEGIN_MSG_MAP_EX(CleanupList)
    NOTIFY_CODE_HANDLER(HDN_ITEMSTATEICONCLICK, OnHeaderCheck)
    REFLECTED_NOTIFY_CODE_HANDLER(LVN_ITEMCHANGED, OnItemChanged)
  END_MSG_MAP()

  void Attach(HWND aListWnd);

  /**
   * Scan for leftover files and populate the list.
   */
  void ScanFiles();

  /**
   * Delete selected files.
   */
  void CleanFiles();

  //----------------------------------------------------------------------------
  // IWorkerThreadObserver iterface members.
  void NotifyWorkDone();

private:

  IPropertyPool & mPropPool;
  UIManager     & mUIManager;
  unique_ptr<WorkerThread> mWorker;

  void DoScanFiles();
  vector<wstring> GetFilesFromTorrent(const FilePath & aTorrentPath);

  void DoCleanFiles();
  
  LRESULT OnHeaderCheck(int, LPNMHDR, BOOL &);
  void SetHeaderCheckbox();

  LRESULT OnItemChanged(int, LPNMHDR, BOOL &);

  void Populate(const vector<wstring> & aFilesList);

  void EnableUI(bool aEnable);
};

#endif // _CLEANUP_LIST_H_
