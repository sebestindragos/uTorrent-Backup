#include "stdafx.h"
#include "WinVersion.h"

//----------------------------------------------------------------------------
// Class definition - public members.
WinVer::WinVer()
{
  Init();
}

WinVer::~WinVer()
{

}

bool WinVer::IsWindowsVista(bool aStrict)
{
  return TestVersion(6, aStrict ? 0 : -1);
}

bool WinVer::IsWindows7(bool aStrict)
{
  return TestVersion(6, aStrict ? 1 : -1);
}

bool WinVer::IsWin64()
{
  WinVer & wndVer = WinVer::Get();
  if (wndVer.mSystemInfo.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64)
    return true;

  return false;
}

//----------------------------------------------------------------------------
// Class definition -  pribate members.
bool WinVer::Init()
{
  ::ZeroMemory(&mOSVersionInfo, sizeof(OSVERSIONINFOEXW));
  mOSVersionInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);

  if (!GetVersionEx((OSVERSIONINFO *)&mOSVersionInfo))
    return false;

  ZeroMemory(&mSystemInfo, sizeof(SYSTEM_INFO));
  GetNativeSystemInfo(&mSystemInfo);

  return true;
}

WinVer &WinVer::Get()
{
  static WinVer wndVersion;
  return wndVersion;
}

bool WinVer::TestVersion(DWORD aMajorVersion, DWORD aMinorVersion)
{
  WinVer & ver = WinVer::Get();

  if (ver.mOSVersionInfo.wProductType != VER_NT_WORKSTATION)
    return false;

  if ( aMajorVersion != ver.mOSVersionInfo.dwMajorVersion || 
       (aMinorVersion != -1 && aMinorVersion != ver.mOSVersionInfo.dwMinorVersion) )
    return false;

  return true;
}

size_t WinVer::GetNumCPUs()
{
  WinVer & ver = WinVer::Get();

  return ver.mSystemInfo.dwNumberOfProcessors;
}
