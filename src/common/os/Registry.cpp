#include "stdafx.h"
#include "Registry.h"

Registry::Registry(HKEY aRootKey, const wstring & aSubkey, bool aAutoCreate, REGSAM aExtraSam)
{
  REGSAM sam = KEY_ALL_ACCESS | aExtraSam;
  if (aAutoCreate)
    ::RegCreateKeyEx(aRootKey, aSubkey.c_str(), NULL, NULL, 0, sam, NULL, &mKey, NULL);
  else
    ::RegOpenKeyEx(aRootKey, aSubkey.c_str(), 0, sam, &mKey);
}

Registry::~Registry()
{
  ::RegCloseKey(mKey);
}

bool Registry::KeyExists()
{
  return mKey != NULL;
}

bool Registry::WriteString(const wstring & aName, const wstring & aValue)
{
  const BYTE * data = reinterpret_cast<const BYTE *>(aValue.data());
  DWORD size = (DWORD)(aValue.size() + 1) * sizeof(wchar_t);
  return ::RegSetValueEx(mKey, aName.c_str(), 0, REG_SZ, data, size) == ERROR_SUCCESS;
}

wstring Registry::ReadString(const wstring & aName)
{
  DWORD size = 0;
  LONG retCode = ::RegQueryValueEx(mKey, aName.c_str(), 0, 0, NULL, &size);

  wstring ret;
  if (retCode == ERROR_SUCCESS && size > 0)
  {
    ret.resize(size + 1);
    ::RegQueryValueEx(mKey, aName.c_str(), 0, 0, reinterpret_cast<LPBYTE>(&ret[0]), &size);
  }

  return ret;
}

bool Registry::WriteBinary(const wstring & aName, const BYTE * aBinary, DWORD aSize)
{
  return ::RegSetValueEx(mKey, aName.c_str(), 0, REG_BINARY, aBinary, aSize) == ERROR_SUCCESS;
}

void Registry::ReadBinary(const wstring & aName, BYTE * aOutput)
{
  DWORD size = 0;
  LONG retCode = ::RegQueryValueEx(mKey, aName.c_str(), 0, 0, NULL, &size);

  if (retCode == ERROR_SUCCESS && size > 0)
  {
    ::RegQueryValueEx(mKey, aName.c_str(), 0, 0, aOutput, &size);
  }
}
