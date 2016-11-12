#ifndef _BENCODE_ITEM_H_
#define _BENCODE_ITEM_H_

/**
 * Class implementing a bEncode item.
 * @author Sebestin Dragos
 */
class bEncodeItem
{
public:

  enum ItemType {Unset, Dictionary, List, Integer, Binary};
 
  typedef deque<bEncodeItem *>::const_iterator const_iterator;
  typedef deque<bEncodeItem *>::const_reverse_iterator const_reverse_iterator;

  friend class bEncodeManager;
  
  /**
   * Member getters and setters.
   */
  void SetLabel(const wstring & aLabel);
  void SetLabel(wstring && aLabel);
  
  void SetValue(const wstring & aValue);
  void SetValue(wstring && aValue);
  
  void SetParent(bEncodeItem * aParent);
  void SetType(ItemType aType);

  void AddChild(bEncodeItem * aChild); 

  ItemType GetType();
  wstring GetLabel();
  wstring GetValue();
  bEncodeItem * GetParent();
  bEncodeItem * GetChild(const wstring & aChildLabel, ItemType aType = Binary);
  size_t GetChildrenCount() const;

  /**
   * Iterators
   */
  const_iterator ChildrenBegin();
  const_iterator ChildrenEnd();
  const_reverse_iterator ChildrenRBegin();
  const_reverse_iterator ChildrenREnd();

  /**
   * Release the node (destroyes the object and also removes the it's reference from it's parent).
   */
  void Release();

private:
  
  /**
   * Hide class constructors and assignment operator.
   * Instances of this class should only be managed by the bEncodeManager.
   */
  bEncodeItem();
  bEncodeItem(const bEncodeItem &);
  bEncodeItem(bEncodeItem &&);
  bEncodeItem & operator = (bEncodeItem);
  ~bEncodeItem();
  
  ItemType               mType;
  wstring                mLabel;
  wstring                mValue;
  bEncodeItem *          mParent;
  deque<bEncodeItem *>   mChildren;

#ifdef _DEBUG
  // leave this in debug mode to check for memory leaks
  static int mNodeCount;
#endif
};

#endif // _BENCODE_ITEM_H_
