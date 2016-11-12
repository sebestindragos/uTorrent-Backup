#ifndef _SYNC_GUARD_H_
#define _SYNC_GUARD_H_

/**
 * Class managing a crititcal section object.
 *
 * @author: Dragos Sebestin
 */
class SyncSupport
{
public:

  SyncSupport();

  ~SyncSupport();

  operator LPCRITICAL_SECTION ();

private:

  // Hide copy constructors and assignment operator.
  SyncSupport(const SyncSupport &);

  SyncSupport & operator = (SyncSupport);
  SyncSupport & operator = (SyncSupport &&);

  CRITICAL_SECTION mCriticalSection;
};

/**
 * Class used for managing mutual exclusion between threads. When sync is needed
 * just create an instance of this object type at the beginning of the critical section.
 *
 * @author: Dragos Sebestin
 */
class SyncGuard
{
public:

  SyncGuard(SyncSupport & aSync);

  ~SyncGuard();

  /**
   * Enter critical section.
   */
  void Lock();

  /**
   * Leave the critical section.
   */
  void Release();

private:

  SyncSupport & mSyncSupport;
};


#endif // _SYNC_GUARD_H_