#include "stdafx.h"
#include "BitPacker.h"

//----------------------------------------------------------------------------
bool BitPacker::IsFlagSet(int aValue, int aFlag)
{
  return ((aValue & aFlag) == aFlag);
}