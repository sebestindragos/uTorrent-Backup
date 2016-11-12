#include "stdafx.h"
#include "ConsoleException.h"

ConsoleException::ConsoleException(const wstring & aMessage)
 : mMessage(aMessage)
{
}

wstring & ConsoleException::What()
{
  return mMessage;
}