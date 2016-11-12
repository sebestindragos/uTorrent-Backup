#include "stdafx.h"
#include "FilePath.h"
#include "StringUtil.h"

using FileUtil::FilePath;

#define BACKSLASH   L'\\'
#define DOT         L'.'

//----------------------------------------------------------------------------
FilePath::FilePath()
{
}

FilePath::FilePath(const FilePath & aOther)
{
  mPath = aOther.mPath;
}

FilePath::FilePath(FilePath && aOther)
{
  mPath.swap(aOther.mPath);
}

FilePath & FilePath::operator = (FilePath aOther)
{
  mPath.swap(aOther.mPath);

  return *this;
}

FilePath::FilePath(const wstring & aPath)
{
  ExtractValidPath(aPath);
}

FilePath & FilePath::Append(const wstring & aPath)
{
  EnsureEndingBackslash();
  mPath.append(aPath);
  ExtractValidPath(mPath);
  
  return *this;
}

const wstring & FilePath::Get() const
{
  return mPath;
}

FilePath FilePath::operator + (const FilePath & aOtherPath)
{
  if (mPath[mPath.size() - 1] != BACKSLASH)
    mPath += BACKSLASH;

  FilePath result(mPath + aOtherPath.mPath);
  return result;
}

FilePath FilePath::GetParent() const
{
  size_t index = 0;
  for (size_t i = mPath.size() - 2; i >= 0; --i)
  {
    if (mPath[i] == L'\\')
    {
      index = i;
      break;
    }
  }
  return FilePath(mPath.substr(0, index + 1));
}

void FilePath::ReplaceExtension(const wstring & aNewExtension)
{
  wstring name = GetName();
  size_t pos = name.rfind(L'.');
  
  if (pos != wstring::npos)
  {
    name.replace(name.begin() + pos + 1, name.end(), 
      aNewExtension.begin(), aNewExtension.end());
  }
  else
  {
    name.append(L"." + aNewExtension);
  }

  pos = mPath.rfind(BACKSLASH);
  if (pos != wstring::npos)
  {
    mPath.replace(mPath.begin() + pos + 1, mPath.end(), name.begin(), name.end());
  }
  else
  {
    mPath = name;
  }
}

void FilePath::EnsureEndingBackslash()
{
  if (mPath.empty())
    return;

  if (*mPath.rbegin() != L'\\')
    mPath += L"\\";
}

wstring FilePath::GetName()
{
  size_t pos = mPath.rfind(BACKSLASH);
  if (pos != wstring::npos)
    return mPath.substr(pos + 1, mPath.size());

  return mPath;
}

wstring FilePath::GetExtension()
{
  auto it = mPath.rbegin();
  auto end = mPath.rend();
  for (; it != end; ++it)
  {
    if (*it == BACKSLASH)
      return wstring();
    if (*it == DOT)
      break;
  }

  wstring ext;
  if (it != end)
    ext = mPath.substr(end - it);

  return ext;
}

bool FilePath::IsAbsolute() const
{
  if (mPath.size() > 3 && mPath[1] == L':')
    return true;

  return false;
}

//----------------------------------------------------------------------------
// Class definition - private members
void FilePath::ExtractValidPath(const wstring & aPath)
{
  if (aPath.size() == 0)
    return;

  vector<wstring> folders = StringUtil::Tokenize(aPath, BACKSLASH);
  if (folders.size() == 0) // nothing to extract
    return;

  // try to construct a valid path
  mPath = folders[0];
  auto it = folders.begin() + 1;
  auto end = folders.end();
  for (; it != end; ++it)
  {
    mPath += BACKSLASH;
    mPath += *it;
  }
}
