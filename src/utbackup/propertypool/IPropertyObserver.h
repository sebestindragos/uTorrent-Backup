#ifndef _IPROPERTY_OBSERVER_H_
#define _IPROPERTY_OBSERVER_H_

#ifndef _STDAFX_INCLUDED_
  #error "Must include stdafx first."
#endif

/**
 *Implements observer pattern for listening to changes in the property pool.
 *
 * @author: Dragos Sebestin
 */
class IPropertyObserver
{
public:

  /**
   * Notification sent when a property changes it's value.
   */
  virtual void PropertyChanged(const wstring & aPropertyName, 
    const wstring & aPropertyValue) = 0;
};

#endif // _IPROPERTY_OBSERVER_H_