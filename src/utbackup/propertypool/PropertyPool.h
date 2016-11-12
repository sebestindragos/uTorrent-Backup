#ifndef _PROPERTY_POOL_H_
#define _PROPERTY_POOL_H_

#include "IPropertyPool.h"

class IPropertyObserver;

/**
 * Property pool class used for holding global properties.
 */
class PropertyPool: public IPropertyPool
{
public:

  PropertyPool();
  ~PropertyPool();

  const wstring & GetProperty(const wstring & aPropName);

  void SetProperty(const wstring & aName, const wstring & aValue);

  void AddObserver(IPropertyObserver * aObserver);

private:

  unordered_map<wstring, wstring> mPropHash; 
  vector<IPropertyObserver *> mObservers;
};

#endif // _PROPERTY_POOL_H_