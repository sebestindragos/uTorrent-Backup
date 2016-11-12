#ifndef _BUFFER_ENCODING_H_
#define _BUFFER_ENCODING_H_

#include "CharsetEncoding.h"

/**
 * Class used for determinig the encoding of a buffer. It uses the Utf8_16Read
 * class by Scott Kirkwood.
 *
 * @author Dragos Sebestin
 */
namespace BufferEncoding
{
  CharsetEncoding::Type GetEncoding(const char * aBuffer, int aBuffLength);
}

#endif // _BUFFER_ENCODING_H_
