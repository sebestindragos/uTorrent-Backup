#ifndef _IWORKER_THREAD_OBSERVER_H_
#define _IWORKER_THREAD_OBSERVER_H_

#ifndef _STDAFX_INCLUDED_
  #error "Must include stdafx first."
#endif

/**
 * WorkertThread observer interface.
 *
 * @author Dragos Sebestin
 */
class IWorkerThreadObserver
{
public:

  virtual void NotifyWorkDone() = 0;
};

#endif // _IWORKER_THREAD_OBSERVER_H_
