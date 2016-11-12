#ifndef _WIN_VERSION_H_
#define _WIN_VERSION_H_

#include <Windows.h>

/**
 * Class used for determining the windows version. 
 *
 * @author : Sebestin Dragos
 */
class WinVer
{
public:

  static bool IsWindowsVista(bool aStrict = false);

  static bool IsWindows7(bool aStrict = false);

  static bool IsWin64();

  static size_t GetNumCPUs();

private:

  WinVer();
  ~WinVer();

  bool Init();
  static WinVer & Get();
  static bool TestVersion(DWORD aMajorVersion, DWORD aMinorVersion = -1);

  OSVERSIONINFOEX mOSVersionInfo;
  SYSTEM_INFO     mSystemInfo;
};

#endif // _WIN_VERSION_H_