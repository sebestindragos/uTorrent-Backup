#include "stdafx.h"
#include "PropertyPool.h"
#include "IPropertyObserver.h"

PropertyPool::PropertyPool()
{
}

PropertyPool::~PropertyPool()
{
}

const wstring & PropertyPool::GetProperty(const wstring & aPropName)
{
  return mPropHash[aPropName];
}

void PropertyPool::SetProperty(const wstring & aName, const wstring & aValue)
{
  mPropHash[aName] = aValue;

  // notify observers
  auto it = mObservers.begin();
  auto end = mObservers.end();
  for (; it != end; ++it)
  {
    (*it)->PropertyChanged(aName, aValue);
  }
}

void PropertyPool::AddObserver(IPropertyObserver * aObserver)
{
  mObservers.push_back(aObserver);
}
