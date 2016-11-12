#include "stdafx.h"
#include "Logger.h"

//----------------------------------------------------------------------------
// Class definition - public members.
Logger & Logger::Get()
{
  static Logger sLogger;
  return sLogger;
}

void Logger::Write(const wstring & aBuffer)
{
  SyncGuard sg(mWriteSync);
  mQueue.push(aBuffer);

  if (mQueue.size() == 1)
    mMonitor.Release();
}

void Logger::WriteLine(const wstring & aBuffer)
{
  Write(aBuffer + L"\n");
}

wstring Logger::GetLine()
{
  if (mQueue.size() == 0)
    mMonitor.Lock();

  SyncGuard sg(mReadSync);
  wstring line = mQueue.back();
  mQueue.pop();

  return line;
}

//----------------------------------------------------------------------------
// Class definition - private members.
Logger::Logger() 
 : mMonitor(mMonitorSync)
{
  mMonitor.Release();
}

Logger::~Logger()
{
  mMonitor.Lock();
}
