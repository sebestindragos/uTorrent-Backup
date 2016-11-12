#include "stdafx.h"
#include "WinException.h"

Exception::Exception()
{
}

Exception::Exception(HWND aWindow, const wstring & aMessage)
{
  mMessage = aMessage;
  mWindow = aWindow;
}

wstring Exception::What()
{
  return mMessage;
}

HWND Exception::Where()
{
  return mWindow;
}
