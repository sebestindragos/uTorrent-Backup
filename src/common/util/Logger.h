#pragma once

#include "SyncGuard.h"

/**
 * Class used for logging application activity to a output stream.
 * Makes use of the singleton pattern.
 *
 * @author: Dragos Sebestin
 */
class Logger
{
public:

  static Logger & Get();

  void Write(const wstring & aBuffer);

  void WriteLine(const wstring & aBuffer);

  wstring GetLine();

private:

  /**
   * Hide class constructors.
   */
  Logger();
  ~Logger();
  
  SyncSupport mWriteSync;
  SyncSupport mReadSync;

  SyncSupport mMonitorSync;
  SyncGuard   mMonitor;
  queue<wstring> mQueue;
};
