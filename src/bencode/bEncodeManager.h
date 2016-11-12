#ifndef _BENCODE_MANAGER_H_
#define _BENCODE_MANAGER_H_

#include "bEncodeItem.h"

/**
 * Class implementing a bEncode manager object.
 * @author Sebestin Dragos
 */
class bEncodeManager
{
public:

  bEncodeManager();
  ~bEncodeManager();
  
  /**
   * Method used for decoding a encoded string into bEncode items.
   */
  bool Decode(const wstring & aEncodedData);

  bool Decode2(const wstring & aEncodedData);

  bool Encode(wstring & aEncodedResultData);

  /**
   * Retrieve the root item
   */
  bEncodeItem * GetRootItem();

  /**
   * Delete all items owned by this object
   */
  void PurgeItems();

  /**
   * Get the bEncode item with a specific label. 
   */
  bEncodeItem * GetItem(const wstring & aItemName, 
    bEncodeItem::ItemType aType = bEncodeItem::Binary);

  /**
   * Get the value of a specific item.
   */
  wstring GetItemValue(const wstring & aItemName, 
    bEncodeItem::ItemType aType = bEncodeItem::Binary);

  /**
   * Add an item to the current item set as iterator
   */
  bEncodeItem * AddChildItem(const wstring & aLabel = L"", 
    const wstring & aValue = L"", bEncodeItem::ItemType aType = bEncodeItem::Binary,
    bool aCloseItem = true);

  /**
   * Method used for moving the internal iterator to the parent of the current item.
   */
  void CloseItem();

private:
  bEncodeItem * mRootItem;
  bEncodeItem * mIteratorItem;

  bool DecodeSegment(const wstring & aRawSegment, wstring & aSegment);

  bEncodeItem * SearchItem(bEncodeItem * aItem, const wstring & aLabel, 
    const wstring & aValue, bEncodeItem::ItemType aType);

  wstring DoEncode(bEncodeItem * aItem);
};


#endif // _BENCODE_MANAGER_H_
