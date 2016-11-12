#include "stdafx.h"
#include "SyncGuard.h"

SyncSupport::SyncSupport()
{
  ::InitializeCriticalSection(&mCriticalSection);
}

SyncSupport::~SyncSupport()
{
  ::DeleteCriticalSection(&mCriticalSection);
}

SyncSupport::operator LPCRITICAL_SECTION()
{
  return &mCriticalSection;
}

//----------------------------------------------------------------------------

SyncGuard::SyncGuard(SyncSupport & aSync): mSyncSupport(aSync)
{
  Lock();
}

SyncGuard::~SyncGuard()
{
  Release();
}

void SyncGuard::Lock()
{
  ::EnterCriticalSection(mSyncSupport);
}

void SyncGuard::Release()
{
  //if (::TryEnterCriticalSection(mSyncSupport) == 0)
    ::LeaveCriticalSection(mSyncSupport);
}
