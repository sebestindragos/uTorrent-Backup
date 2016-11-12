#ifndef _BUFFER_CONVERTER_H_
#define _BUFFER_CONVERTER_H_

#include "CharsetEncoding.h"

/**
 * Namespace used for converting a buffer between encodings.
 *
 * @author Dragos Sebestin
 */
namespace BufferConverter
{
  wstring ToUnicode(const char * aBuffer, int aBufferLen);

  string FromUnicode(const wstring & aBuffer, CharsetEncoding::Type aEncoding);
}

#endif // _BUFFER_CONVERTER_H_
