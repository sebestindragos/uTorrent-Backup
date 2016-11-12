#ifndef _BIT_PACKER_H_
#define _BIT_PACKER_H_

/**
 * Utility methods for manipulating bits.
 *
 * @author Dragos Sebestin
 */
namespace BitPacker
{
  /**
   * Check if a certain flag is set in a value.
   */
  bool IsFlagSet(int aValue, int aFlag);

} // BitPacker

#endif // _BIT_PACKER_H_