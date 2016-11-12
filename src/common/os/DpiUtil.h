#ifndef _DPI_UTIL_H_
#define _DPI_UTIL_H_

/**
 * Namespace containing various dpi scaling methods. 
 *
 * @author: Dragos Sebestin
 */
namespace DpiUtil
{
  /**
   * Perform a scaling on the given value based on the detected system dpi.
   */
  int AdjustValueToCurrentDpi(int aValue);
}

#endif // _DPI_UTIL_H_
