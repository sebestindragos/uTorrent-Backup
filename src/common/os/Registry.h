#ifndef _REGISTRY_H_
#define _REGISTRY_H_

/**
 * Class implementing an object for manipulating registry entries.
 *
 * @author Dragos Sebestin
 */
class Registry
{
public:

  Registry(HKEY aRootKey, const wstring & aSubkey, bool aAutoCreate = false,
    REGSAM aExtraSam = NULL);
  ~Registry();

  bool KeyExists();

  bool WriteString(const wstring & aName, const wstring & aValue);

  wstring ReadString(const wstring & aName);

  bool WriteBinary(const wstring & aName, const BYTE * aBinary, DWORD aSize);

  void ReadBinary(const wstring & aName, BYTE * aOutput);

private:

  HKEY mKey;
};

#endif // _REGISTRY_H_