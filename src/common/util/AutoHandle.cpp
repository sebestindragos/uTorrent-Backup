#include "stdafx.h"
#include "AutoHandle.h"

//----------------------------------------------------------------------------
// Implementation of AutoHandle class methods.
AutoHandle::AutoHandle() : mHandle(INVALID_HANDLE_VALUE)
{
}

AutoHandle::AutoHandle(HANDLE aHandle) : mHandle(aHandle)
{
}

AutoHandle::~AutoHandle()
{
  ::CloseHandle(mHandle);
}

HANDLE AutoHandle::operator=(HANDLE aHandle)
{
  if (mHandle != INVALID_HANDLE_VALUE)
    ::CloseHandle(mHandle);
  mHandle = aHandle;
  return mHandle;
}

AutoHandle::operator HANDLE()
{
  return mHandle;
}
//----------------------------------------------------------------------------
