#ifndef _STRING_UTIL_H_
#define _STRING_UTIL_H_

namespace StringUtil
{
  int StringToInt(const wstring & aString);

  template <typename IntegerType>
  wstring IntToString(IntegerType aInt)
  {
    short numChars = 0;
    IntegerType aIntCopy = aInt;
    do
    {
      numChars++;
      aIntCopy /= 10;
    }while (aIntCopy);
    if (aInt < 0)
      numChars++;

    wstring result;
    result.resize(numChars);
#ifdef _M_IX86
    _itot_s(aInt, &result[0], numChars + 1, 10);
#else
    _i64tot_s(aInt, &result[0], numChars + 1, 10);
#endif
    

    return result;
  }

  vector<wstring> Tokenize(const wstring & aString, wchar_t aToken);

  wstring Merge(const vector<wstring> & aTokens, const wstring & aSeparator = L"");

  wstring StripQuotes(const wstring & aString);

  wstring ByteArrayToHEXString(const unsigned char * aByteArray, size_t aArraySize);

  wstring CPToUnicode(const char * aBuffer, size_t aBuffLen, UINT aCP);

  string UnicodeToCP(const wstring & aBuffer, UINT aCP);

}; // StringUtil 

#endif // _STRING_UTIL_H_