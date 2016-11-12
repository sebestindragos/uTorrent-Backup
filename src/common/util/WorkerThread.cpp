#include "stdafx.h"
#include "WorkerThread.h"
#include "IWorkerThreadObserver.h"

//----------------------------------------------------------------------------
// Class definition - public members.
WorkerThread::WorkerThread() : mName(L"Worker")
{

}

void WorkerThread::AddObserver(IWorkerThreadObserver * aObserver)
{
  mObservers.push_back(aObserver);
}

void WorkerThread::DoWorkAsync(WorkerFunc aFunc)
{
  try
  {
    mWorkerFunc = aFunc;
    
    // start the background thread
    mThreadHandle = ::CreateThread(NULL, 0, 
      reinterpret_cast<LPTHREAD_START_ROUTINE>(&WorkerThread::ThreadProc), 
      reinterpret_cast<LPVOID>(this), CREATE_SUSPENDED, NULL);
    ::ResumeThread(mThreadHandle);
  }
  CATCH_EVERYTHING;
}

//----------------------------------------------------------------------------
// Class definition - private members.
DWORD WorkerThread::ThreadProc(LPVOID aData)
{
  WorkerThread * self = reinterpret_cast<WorkerThread *>(aData);

  try
  {
    // execute the required operation
    self->mWorkerFunc();
  }
  CATCH_EVERYTHING;

  try
  {
    // notify observers that the operation has ended
    auto it = self->mObservers.begin();
    auto end = self->mObservers.end();
    for (; it != end; ++it)
    {
      (*it)->NotifyWorkDone();
    }
  }
  CATCH_EVERYTHING;

  return 0;
}
