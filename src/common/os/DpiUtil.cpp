#include "stdafx.h"
#include "DpiUtil.h"

namespace
{
  const int kSystemDpi = ::GetDeviceCaps(::GetDC(::GetDesktopWindow()), LOGPIXELSY);
  const int kNormalDpi = 96;
}

//----------------------------------------------------------------------------
int DpiUtil::AdjustValueToCurrentDpi(int aValue)
{
  float scaledValue = (float)aValue * kSystemDpi / kNormalDpi;
  return (int) scaledValue;
}