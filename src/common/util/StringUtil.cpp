#include "stdafx.h"
#include "StringUtil.h"

#define QUOTE L'\"'

int StringUtil::StringToInt(const wstring & aString)
{
  return _ttoi(aString.c_str());
}

vector<wstring> StringUtil::Tokenize(const wstring & aString, wchar_t aToken)
{
  vector<wstring> result;

  size_t pos = 0;
  for ( size_t i = 0; i < aString.size(); i++)
  {
    if (aString[i] == aToken)
    {
      result.push_back(aString.substr(pos, i - pos));
      pos = i + 1;
    }
  }
  if (pos < aString.size())
    result.push_back(aString.substr(pos));

  return result;
}

wstring StringUtil::Merge(const vector<wstring> & aTokens, const wstring & aSeparator)
{
  if (aTokens.size() == 0)
    return L"";

  wstring merged;
  auto it = aTokens.begin();
  auto end = --aTokens.end();
  for (; it != end; ++it)
  {
    merged += *it;
    merged += aSeparator;
  }

  merged += *aTokens.rbegin();

  return merged;
}

wstring StringUtil::StripQuotes(const wstring & aString)
{
  size_t start = 0;
  size_t end = aString.size() - 1;

  if (aString[start] == QUOTE)
    start++;
  if (aString[end] == QUOTE)
    end--;

  return wstring(aString, start, end);
}

wstring StringUtil::ByteArrayToHEXString(const unsigned char * aByteArray, size_t aArraySize)
{
  auto GetHEXChar = [](unsigned char aValue) -> wchar_t
  {
    if (aValue >= 0 && aValue <= 9) // 0 - 9
      return (wchar_t)(aValue | 48);
    else
      return (wchar_t)(aValue + 55);
  };

  wstring hexString;
  hexString.reserve(aArraySize * 2);
  for (size_t i = 0; i < aArraySize; ++i)
  {
    unsigned char half = (aByteArray[i] & 0xf0) >> 4;
    hexString.push_back(GetHEXChar(half));

    half = aByteArray[i] & 0x0f;
    hexString.push_back(GetHEXChar(half));
  }

  return hexString;
}

wstring StringUtil::CPToUnicode(const char * aBuffer, size_t aBuffLen, UINT aCP)
{
  int resLen = ::MultiByteToWideChar(aCP, MB_PRECOMPOSED, aBuffer, (int)aBuffLen, nullptr, 0);

  wstring result;
  result.resize(resLen);

  ::MultiByteToWideChar(aCP, MB_PRECOMPOSED, aBuffer, (int)aBuffLen, &result[0], resLen);

  return result;
}

string StringUtil::UnicodeToCP(const wstring & aBuffer, UINT aCP)
{
  int narrowLen = ::WideCharToMultiByte(aCP, WC_COMPOSITECHECK, aBuffer.data(), 
                                        (int)aBuffer.size(), nullptr, 0, NULL, NULL);

  string result;
  result.resize(narrowLen);

  ::WideCharToMultiByte(aCP, WC_COMPOSITECHECK, aBuffer.data(), 
                        (int)aBuffer.size(), &result[0], narrowLen, NULL, NULL);

  return result;
}
