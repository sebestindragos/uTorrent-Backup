#include "stdafx.h"
#include "bEncodeItem.h"

bEncodeItem::bEncodeItem() : mType(Unset), mParent(nullptr)
{
#ifdef _DEBUG
  mNodeCount++;
#endif
}

bEncodeItem::~bEncodeItem()
{
#ifdef _DEBUG
  mNodeCount--;
#endif
  auto it = ChildrenBegin();
  auto end = ChildrenEnd();
  for (; it != end; ++it)
    delete *it;
}

void bEncodeItem::SetType(ItemType aType)
{
  mType = aType;
}

void bEncodeItem::SetLabel(const wstring & aLabel)
{
  mLabel = aLabel;
}

void bEncodeItem::SetLabel(wstring && aLabel)
{
  std::swap(mLabel, aLabel);
}

void bEncodeItem::SetValue(const wstring & aValue)
{
  mValue = aValue;
}

void bEncodeItem::SetValue(wstring && aValue)
{
  std::swap(mValue, aValue);
}

void bEncodeItem::SetParent(bEncodeItem * aParent)
{
  mParent = aParent;
}

void bEncodeItem::AddChild(bEncodeItem * aChild)
{
  aChild->SetParent(this);
  mChildren.push_back(aChild);
}

bEncodeItem::ItemType bEncodeItem::GetType()
{
  return mType;
}

wstring bEncodeItem::GetLabel()
{
  return mLabel;
}

wstring bEncodeItem::GetValue()
{
  return mValue;
}

bEncodeItem * bEncodeItem::GetParent()
{
  return mParent;
}

bEncodeItem * bEncodeItem::GetChild(const wstring & aChildLabel, ItemType aType)
{
  auto it = ChildrenBegin();
  auto end = ChildrenEnd();
  for (; it != end; ++it)
  {
    bEncodeItem * crtChild = *it;
    if (crtChild->GetLabel() == aChildLabel && crtChild->GetType() == aType)
      return crtChild;
  }

  return nullptr;
}

size_t bEncodeItem::GetChildrenCount() const
{
  return mChildren.size();
}

bEncodeItem::const_iterator bEncodeItem::ChildrenBegin()
{
  return mChildren.begin();
}

bEncodeItem::const_iterator bEncodeItem::ChildrenEnd()
{
  return mChildren.end();
}

bEncodeItem::const_reverse_iterator bEncodeItem::ChildrenRBegin()
{
  return mChildren.rbegin();
}

bEncodeItem::const_reverse_iterator bEncodeItem::ChildrenREnd()
{
  return mChildren.rend();
}

void bEncodeItem::Release()
{
  if (mParent)
  {
    // remove parent ref
    auto it = mParent->ChildrenBegin();
    auto end = mParent->ChildrenEnd();
    for (; it != end; ++it)
    {
      if (*it == this)
      {
        mParent->mChildren.erase(it);
        break;
      }
    }
  }

  // physically delete this node.
  delete this;
}

#ifdef _DEBUG
int bEncodeItem::mNodeCount = 0;
#endif