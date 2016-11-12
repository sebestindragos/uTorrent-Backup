#include "stdafx.h"
#include "bEncodeManager.h"
#include "common/util/StringUtil.h"

#define BENCODE_END_MARKER           L'e'
#define BENCODE_LENGTH_SEPARATOR     L':'

#define BENCODE_DICTIONARY_MARKER    L'd'
#define BENCODE_LIST_MARKER          L'l'
#define BENCODE_INTEGER_MARKER       L'i'

bEncodeManager::bEncodeManager()
 : mRootItem(nullptr), mIteratorItem(nullptr)
{
}

bEncodeManager::~bEncodeManager()
{
  PurgeItems();
}

void bEncodeManager::PurgeItems()
{
  if (mRootItem)
    delete mRootItem;
  mRootItem = nullptr;
}

bool bEncodeManager::Decode(const wstring & aEncodedData)
{
  bEncodeItem * crtItem = new bEncodeItem;
  bEncodeItem * parentItem = crtItem;
  int depth = 0;
  size_t i = 0;

  for (; i < aEncodedData.length() && parentItem; i++)
  {
    if ( crtItem && (crtItem->GetType() != bEncodeItem::Unset))
      crtItem = new bEncodeItem;

    // store the root item
    if (i == 0)
      mRootItem = crtItem;

    switch(aEncodedData[i])
    {
    case L'd':
      crtItem->SetType(bEncodeItem::Dictionary);
      depth = 1;
      break;
    case L'l':
      crtItem->SetType(bEncodeItem::List);
      depth = 1;
      break;
    case L'e':
      depth = -1;
      break;
    default:
      {
        depth = 0;

        // decode segment
        wstring label;
        if (!DecodeSegment(wstring(&aEncodedData[i], aEncodedData.length() - i), label))
          return false;
        crtItem->SetLabel(label);

        // update position in buffer
        auto UpdatePosition = [&](const wstring & aSegment) -> bool
        {
          int offset = aEncodedData[i] == L'i' ? 1 : 0;
          std::size_t pos;
          pos = aEncodedData.find(aSegment, i + 1);
          if (pos == wstring::npos)
            return false;
          i = pos + aSegment.length() + offset;
          return true;
        };

        if (!UpdatePosition(label))
          return false;

        // get item type
        auto GetNextItemType = [&aEncodedData, &i]() -> bEncodeItem::ItemType
        {
          return aEncodedData[i] == L'i' ? bEncodeItem::Integer : bEncodeItem::Binary;;
        };
        bEncodeItem::ItemType type = GetNextItemType();
        // this is needed for the "0:" case because value has no length
        if (type == bEncodeItem::Binary && label.length() == 0)
          i++;

        // check if the next item is a list or a dictionary, case in which
        // we must leave the type unset and break. 
        if (aEncodedData[i] == L'd' || aEncodedData[i] == L'l')
        {
          i--;
          break;
        }
        crtItem->SetType(type);

        // check if the item is inside a list
        if (parentItem->GetType() == bEncodeItem::List)
        {
          i--;
          break;
        }

        //decode segment
        wstring value;
        if (!DecodeSegment(wstring(&aEncodedData[i], aEncodedData.length() - i), value))
          return false;
        crtItem->SetValue(value);

        // update position in buffer
        if (!UpdatePosition(value))
          return false;

        type = GetNextItemType();
        crtItem->SetType(type);

        // this is needed for the "0:" case because value has no length
        if (type == bEncodeItem::Binary && value.length() == 0)
          i++;

        i--;
      }
    }

    if ( (crtItem->GetType() != bEncodeItem::Unset) && 
      crtItem != mRootItem)
      parentItem->AddChild(crtItem);

    if (depth == 1)
      parentItem = crtItem;
    else if (depth == -1)
      parentItem = parentItem->GetParent();

  }
  
  if (i != aEncodedData.length() || parentItem != nullptr)
    return false;

  mIteratorItem = mRootItem;

  return true;
}

bEncodeItem * bEncodeManager::GetRootItem()
{
  return mRootItem;
}

bool bEncodeManager::DecodeSegment(const wstring & aRawSegment, wstring & aSegment)
{
  if ( (aRawSegment[0] < 48 || aRawSegment[0] > 57) && aRawSegment[0] != L'i')
    return false;

  if (aRawSegment[0] == L'i')
  {
    size_t endMarkerPos = aRawSegment.find(BENCODE_END_MARKER);
    if (endMarkerPos == wstring::npos)
      return false;

    aSegment = aRawSegment.substr(1, endMarkerPos - 1);
    if (aSegment.length() != endMarkerPos - 1)
      return false;
  } else 
  {
    int segmentLength = _ttoi(aRawSegment.c_str());
    if (segmentLength < 0)
      return false;

    size_t separatorPos = aRawSegment.find(BENCODE_LENGTH_SEPARATOR);

    aSegment.resize(segmentLength);
    for (int i = 0; i < segmentLength; i++)
      aSegment[i] = aRawSegment[separatorPos + 1 + i];

    //aSegment = aRawSegment.substr(separatorPos + 1, segmentLength);

    if ((int)aSegment.length() != segmentLength)
      return false;
  }

  return true;
}

bool bEncodeManager::Decode2(const wstring & aEncodedData)
{
  // cleanup old items
  PurgeItems();

  /**
   * A bEncoded file must always have a root element which will be a dictionary.
   */
  bEncodeItem * parentItem (new bEncodeItem);
  parentItem->SetType(bEncodeItem::Dictionary);
  mRootItem = parentItem;

  // perform safety checks
  if (aEncodedData[0] != BENCODE_DICTIONARY_MARKER)
    return false;
  if (aEncodedData.length() < 2)
    return false;

  // place root item type on stack
  stack<bEncodeItem::ItemType> itemTypeStack;
  bEncodeItem::ItemType itemType = bEncodeItem::Dictionary;
  itemTypeStack.push(itemType);

  // begin parsing the buffer
  bEncodeItem * crtItem(new bEncodeItem);
  size_t bufferIndex = 1;
  const wchar_t * encodedBuffer = &aEncodedData[0];
  bool expectRValue = false;
  for (; bufferIndex < aEncodedData.length() && !itemTypeStack.empty(); bufferIndex++)
  {

    if (crtItem->GetType() != bEncodeItem::Unset && !expectRValue)
    {
      crtItem = new bEncodeItem;
    }

    auto GoToMarker = [&](wchar_t aMarker, bool aPostIncrement)
    {
      while (encodedBuffer[bufferIndex] != aMarker)
        bufferIndex++;
      if (aPostIncrement)
        bufferIndex++;
    };
    int depth = 0;
    switch (encodedBuffer[bufferIndex])
    {
      case BENCODE_DICTIONARY_MARKER:
        itemType = bEncodeItem::Dictionary;
        crtItem->SetType(itemType);
        itemTypeStack.push(itemType);
        expectRValue = false;
        depth = 1;
        break;

      case BENCODE_LIST_MARKER:
        itemType = bEncodeItem::List;
        crtItem->SetType(itemType);
        itemTypeStack.push(itemType);
        expectRValue = false;
        depth = 1;
        break;

      case BENCODE_INTEGER_MARKER:
        {
          crtItem->SetType(bEncodeItem::Integer);
          int integerLength = 0;
          while (encodedBuffer[bufferIndex + integerLength] != BENCODE_END_MARKER)
          {
            integerLength++;
          }
          wstring intSegment(encodedBuffer + bufferIndex + 1, integerLength - 1);
          if (!expectRValue)
          {
            crtItem->SetLabel(std::move(intSegment));
          }
          else
          {
            crtItem->SetValue(std::move(intSegment));
            expectRValue = false;
          }
          GoToMarker(BENCODE_END_MARKER, false);
        }
        break;

      case BENCODE_END_MARKER:
        itemType = itemTypeStack.top();
        itemTypeStack.pop();
        expectRValue = false;
        depth = -1;
        break;

      default:
      {
        int segmentLength = _ttoi(encodedBuffer + bufferIndex);
        GoToMarker(BENCODE_LENGTH_SEPARATOR, true);

        wstring segment = wstring(encodedBuffer + bufferIndex, segmentLength);
        bufferIndex += segmentLength - 1;
        if (!expectRValue)
          crtItem->SetLabel(std::move(segment));
        else
          crtItem->SetValue(std::move(segment));

        crtItem->SetType(bEncodeItem::Binary);
        if ( expectRValue == false && 
             (itemTypeStack.top() != bEncodeItem::List) )
        {
          expectRValue =  true;
        }
        else
          expectRValue = false;
      }
    }

    if (crtItem->GetType() != bEncodeItem::Unset && !expectRValue)
      parentItem->AddChild(crtItem);

    if (depth == 1)
      parentItem = crtItem;
    else if (depth == -1)
    {
      parentItem = parentItem->GetParent();
    }
  }

  if (crtItem && crtItem->GetType() == bEncodeItem::Unset)
    delete crtItem;

  if ( bufferIndex != aEncodedData.length() ||
       itemTypeStack.size() != 0 )
  {
    PurgeItems();
    return false;
  }

  return true;
}

bool bEncodeManager::Encode(wstring & aEncodedResultData)
{
  aEncodedResultData.clear();

  aEncodedResultData = DoEncode(mRootItem);

  return true;
}

wstring bEncodeManager::DoEncode(bEncodeItem * aItem)
{
  wstring encodedItem;
  
  if (!aItem)
    return encodedItem;

  auto AppendMarker = [&](wchar_t aMarker) -> void
  {
    wchar_t markerStr[2] = {aMarker, 0};
    encodedItem.append(markerStr);
  };

  auto WithLength = [&aItem](wstring aString) -> wstring
  {
    if (aString.size() == 0)
      if (aItem->GetType() == bEncodeItem::Binary)
        return L"0:";
      else
        return wstring();
    wstring strWithLength(StringUtil::IntToString(aString.size()));
    wchar_t markerStr[2] = {BENCODE_LENGTH_SEPARATOR, 0};
    
    strWithLength.append(markerStr);
    strWithLength.append(aString);

    return strWithLength;
  };

  bool needEndMarker = false;
  switch (aItem->GetType())
  {

  case bEncodeItem::Dictionary:
    encodedItem.append(WithLength(aItem->GetLabel()));
    AppendMarker(BENCODE_DICTIONARY_MARKER);
    needEndMarker = true;
    break;

  case bEncodeItem::List:
    encodedItem.append(WithLength(aItem->GetLabel()));
    AppendMarker(BENCODE_LIST_MARKER);
    needEndMarker = true;
    break;

  case bEncodeItem::Integer:
    if (aItem->GetValue() == L"")
    {
      AppendMarker(BENCODE_INTEGER_MARKER);
      encodedItem.append(aItem->GetLabel());
    }
    else
    {
      encodedItem.append(WithLength(aItem->GetLabel()));
      AppendMarker(BENCODE_INTEGER_MARKER);
      encodedItem.append(aItem->GetValue());
    }
    needEndMarker = true;
    break;
    
  case bEncodeItem::Binary:
    encodedItem.append(WithLength(aItem->GetLabel()));
    encodedItem.append(WithLength(aItem->GetValue()));
    break;
  }

  auto it = aItem->ChildrenBegin();
  auto end = aItem->ChildrenEnd();
  for (; it != end; ++it)
  {
    encodedItem.append(DoEncode(*it));
  }

  if (needEndMarker)
    AppendMarker(BENCODE_END_MARKER);

  return encodedItem;
}

wstring bEncodeManager::GetItemValue(const wstring & aItemName, 
                                     bEncodeItem::ItemType aType)
{
  bEncodeItem * item = SearchItem(mRootItem, aItemName, wstring(), aType);

  return item != nullptr ? item->GetValue() : wstring();
}

bEncodeItem * bEncodeManager::SearchItem(bEncodeItem * aItem, 
                                         const wstring & aLabel, 
                                         const wstring & aValue, 
                                         bEncodeItem::ItemType aType)
{
  bool match = false;
  
  match |=  ( (aLabel.size() == 0 || aLabel == aItem->GetLabel()) &&
              (aValue.size() == 0 || aValue == aItem->GetValue()) &&
              (aType == aItem->GetType()) );

  if (match)
    return aItem;

  auto it = aItem->ChildrenBegin();
  auto end = aItem->ChildrenEnd();
  for (; it != end; ++it)
  {
    bEncodeItem * item = SearchItem(*it, aLabel, aValue, aType);
    if (item)
      return item;
  }

  return nullptr;
}

bEncodeItem * bEncodeManager::AddChildItem(const wstring & aLabel,
                                           const wstring & aValue, 
                                           bEncodeItem::ItemType aType,
                                           bool aCloseItem)
{
  if (mRootItem == nullptr)
  {
    mRootItem = new bEncodeItem;
    mRootItem->SetType(bEncodeItem::Dictionary);
    mIteratorItem = mRootItem;
  }

  if (!mIteratorItem)
    mIteratorItem = mRootItem;

  bEncodeItem * itemPtr = new bEncodeItem;
  
  itemPtr->SetLabel(aLabel);
  itemPtr->SetValue(aValue);
  itemPtr->SetType(aType);

  mIteratorItem->AddChild(itemPtr);
  if (!aCloseItem)
    mIteratorItem = itemPtr;

  return itemPtr;
}

bEncodeItem * bEncodeManager::GetItem(const wstring & aItemName, 
                                      bEncodeItem::ItemType aType)
{
  return SearchItem(mRootItem, aItemName, wstring(), aType);
}

void bEncodeManager::CloseItem()
{
  if (mIteratorItem)
    mIteratorItem = mIteratorItem->GetParent();
}
