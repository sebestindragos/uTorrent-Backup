#ifndef _WORKER_THREAD_H_
#define _WORKER_THREAD_H_

#ifndef _STDAFX_INCLUDED_
  #error "Must include stdafx first."
#endif

#include "common/util/AutoHandle.h"

class IWorkerThreadObserver;

/**
 * Class used for performing operations on a background thread, leaving the UI thread free.
 *
 * @author Dragos Sebestin
 */
class WorkerThread
{
public:

  typedef std::function<void(void)> WorkerFunc;

  /**
   * Class constructor.
   */
  WorkerThread();

  void AddObserver(IWorkerThreadObserver * aObserver);

  void DoWorkAsync(WorkerFunc aFunc);

private:
  
  AutoHandle mThreadHandle;
  WorkerFunc mWorkerFunc;
  wstring mName;

  vector<IWorkerThreadObserver *> mObservers; 

  static DWORD WINAPI ThreadProc(LPVOID aData);
};

#endif // _WORKER_THREAD_H_
