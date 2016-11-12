#include "stdafx.h"
#include "File.h"
#include "../os/BufferConverter.h"

using namespace FileUtil;

//----------------------------------------------------------------------------
// Implementation of File class methods.
File::File(const wstring & aFilePath, bool aAutoCreate, CharsetEncoding::Type aEncoding) 
  : mFilePath(aFilePath), mExists(true), mEncoding(aEncoding)
{
  DWORD createDisposition = (aAutoCreate ? CREATE_ALWAYS : OPEN_EXISTING);

  mFileHandle = CreateFile(mFilePath.c_str(), GENERIC_READ | GENERIC_WRITE,
    FILE_SHARE_READ, NULL, createDisposition, NULL, NULL);
  if (mFileHandle == INVALID_HANDLE_VALUE)
    mExists = false;
}

bool File::IsValid()
{
  return mExists;
}

wstring File::Read()
{
  if (!mExists)
    return L"";

  DWORD fileSize = GetFileSize(mFileHandle, NULL);
  DWORD bytesRead = 0;
  vector<char> data;
  data.resize(fileSize);

  if (::ReadFile(mFileHandle, (LPVOID)&data[0], fileSize, &bytesRead, NULL) == 0)
  {
    return L"";
  }
  if (bytesRead != fileSize)
    return L"";
    
  return BufferConverter::ToUnicode(&data[0], fileSize);
}

bool File::Write(const wstring & aContents)
{
  ::SetFilePointer(mFileHandle, NULL, NULL, FILE_BEGIN);
  return WriteContents(aContents, false);
}

bool File::Append(const wstring & aContents)
{
  ::SetFilePointer(mFileHandle, NULL, NULL, FILE_END);
  return WriteContents(aContents, true);
}

LONGLONG File::GetSize()
{
  LARGE_INTEGER size;
  ::GetFileSizeEx(mFileHandle, &size);
  return size.QuadPart;
}

//----------------------------------------------------------------------------
// Private methods.
bool File::WriteContents(const wstring & aContents, bool /*aAppend*/)
{
  if (!mExists)
    return false;
  
  string buffer = BufferConverter::FromUnicode(aContents, mEncoding);

  // write buffer to file
  DWORD bytesWritten = 0;
  ::WriteFile(mFileHandle, buffer.data(), (DWORD)buffer.size(), &bytesWritten, NULL);

  if (bytesWritten < buffer.size())
    return false;
  return true;
}

void File::Close()
{
  mFileHandle = INVALID_HANDLE_VALUE;
  mExists = false;
}