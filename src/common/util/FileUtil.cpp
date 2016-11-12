#include "stdafx.h"
#include "FileUtil.h"
#include "FilePath.h"

using namespace FileUtil;


void FileUtil::RemoveDirectoryAndContents(const wstring & aDirPath)
{
  wstring validDirPath = aDirPath;
  if (*validDirPath.rbegin() != L'\\')
    validDirPath += L"\\";
  WIN32_FIND_DATA findFileData;
  HANDLE hFind = ::FindFirstFile((validDirPath + L"*.*").c_str(), &findFileData);
  if (hFind == INVALID_HANDLE_VALUE)
    return;

  do 
  {
    wstring fileName = findFileData.cFileName;
    if (fileName != L"." && fileName != L"..")
    {
      if (findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
        RemoveDirectoryAndContents(aDirPath + fileName);
      else
      {
        ::DeleteFile((validDirPath + fileName).c_str());
      }
    }
    
  } while (FindNextFile(hFind, &findFileData));

  ::FindClose(hFind);

  ::RemoveDirectory(validDirPath.c_str());
}

void FileUtil::ListDirectoryContents(const wstring & aPath, vector<wstring> & aFiles, 
                                     bool aRecursive, const wstring & aExtension,
                                     bool aFullPath, bool aRelativePath)
{
  // construct the path
  FilePath path(aPath);
  path.EnsureEndingBackslash();
  
  // begin the search
  WIN32_FIND_DATA findFileData;
  HANDLE find = ::FindFirstFile((path.Get() + L"*." + aExtension).c_str(), &findFileData);
  if (find == INVALID_HANDLE_VALUE)
    return;

  do 
  {
    wstring fileName = findFileData.cFileName;
    if (fileName != L"." && fileName != L"..")
    {
      if (aRecursive && (findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
      {
        if (!aRelativePath)
          ListDirectoryContents(path.Get() + fileName, aFiles, aRecursive, aExtension, aFullPath, aRelativePath);
        else
        {
          vector<wstring> files;
          ListDirectoryContents(path.Get() + fileName, files, aRecursive, aExtension, aFullPath, aRelativePath);
          for_each(files.begin(), files.end(), 
          [&](const wstring & aFile)
          {
            FilePath path = fileName;
            path.Append(aFile);
            aFiles.push_back(path.Get());
          });
        }
      }
      else if (!(findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
      {
        // get the file extension
        wstring ext = wstring(fileName, fileName.find_last_of(L'.') + 1);
        if (aExtension == L"*" || ext == aExtension)
        {
          if (aFullPath)
            aFiles.push_back(path.Get() + fileName);
          else
            aFiles.push_back(fileName);
        }
      }
    }
  } while (::FindNextFile(find, &findFileData));

  ::FindClose(find);
}
