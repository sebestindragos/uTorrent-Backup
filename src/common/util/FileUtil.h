#ifndef _FILEUTIL_H_
#define _FILEUTIL_H_

namespace FileUtil
{
  /**
   * Remove a folder and it's contents recursively.
   */
  void RemoveDirectoryAndContents(const wstring & aDirPath);

  /**
   * List the contents of a folder.
   */
  void ListDirectoryContents(const wstring & aPath, vector<wstring> & aFiles, 
    bool aRecursive = false, const wstring & aExtension = L"*", bool aFullPath = false,
    bool aRelativePath = false);

}; // FileUtil
#endif // _FILEUTIL_H_
