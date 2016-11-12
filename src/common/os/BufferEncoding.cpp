#include "stdafx.h"
#include "BufferEncoding.h"


#include "../third-party/npp/Utf8_16.h"
//----------------------------------------------------------------------------
CharsetEncoding::Type BufferEncoding::GetEncoding(const char * aBuffer, int aBuffLength)
{
  Utf8_16_Read npp;
  npp.determineEncoding((const unsigned char *)aBuffer, aBuffLength);

  switch (npp.getEncoding())
  {
  case uni8Bit:
  case uni7Bit:
    return CharsetEncoding::Ansi;

  case uniUTF8:
    return CharsetEncoding::Utf8;

  case uniCookie:
    return CharsetEncoding::Utf8_NoBom;

  case uni16LE:
    return CharsetEncoding::Utf16LE;

  case uni16LE_NoBOM:
      return CharsetEncoding::Utf16LE_NoBom;

  default:
    ATLASSERT(!L"Unknown encoding.");
    return CharsetEncoding::Utf8;
  }
}