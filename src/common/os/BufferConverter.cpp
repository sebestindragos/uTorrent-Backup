#include "stdafx.h"
#include "BufferConverter.h"
#include "BufferEncoding.h"
#include "../util/StringUtil.h"

//----------------------------------------------------------------------------
wstring BufferConverter::ToUnicode(const char * aBuffer, int aBufferLen)
{
  CharsetEncoding::Type encoding = BufferEncoding::GetEncoding(aBuffer, aBufferLen);

  switch (encoding)
  {
  case CharsetEncoding::Ansi:
    return StringUtil::CPToUnicode(aBuffer, aBufferLen, CP_ACP);

  case CharsetEncoding::Utf8:
    return StringUtil::CPToUnicode(aBuffer, aBufferLen, CP_UTF8);

  case CharsetEncoding::Utf8_NoBom:
    return StringUtil::CPToUnicode(aBuffer + 3, aBufferLen - 3, CP_UTF8);

  case CharsetEncoding::Utf16LE:
    return wstring(reinterpret_cast<const wchar_t *>(aBuffer + 2), aBufferLen - 2);

  case CharsetEncoding::Utf16LE_NoBom:
    return wstring(reinterpret_cast<const wchar_t *>(aBuffer), aBufferLen);

  default:
    ATLASSERT(!L"Unknown encoding");
    return wstring();
  }
}

string BufferConverter::FromUnicode(const wstring & aBuffer, CharsetEncoding::Type aEncoding)
{
  switch (aEncoding)
  {
  case CharsetEncoding::Ansi:
    return StringUtil::UnicodeToCP(aBuffer, CP_ACP);

  case CharsetEncoding::Utf8:
  case CharsetEncoding::Utf8_NoBom:
    return StringUtil::UnicodeToCP(aBuffer, CP_UTF8);

  case CharsetEncoding::Utf16LE:
  case CharsetEncoding::Utf16LE_NoBom:
    return string(reinterpret_cast<const char *>(aBuffer.data()), aBuffer.length());

  default:
    ATLASSERT(!L"Unknown encoding");
    return string();
  }
}