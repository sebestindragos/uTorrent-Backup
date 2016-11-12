#ifndef _FILE_H_
#define _FILE_H_

#include "AutoHandle.h"
#include "../os/CharsetEncoding.h"

namespace FileUtil
{
  /**
    * Class implementing a file object. Used for reading file contents and 
    * different file operations.
    *
    * @author : Sebestin Dragos
    */
  class File
  {
  public:

    File(const wstring & aFilePath, bool aAutoCreate = false, 
         CharsetEncoding::Type aEncoding = CharsetEncoding::Ansi);

    bool IsValid();
    wstring Read();
    bool Write(const wstring & aContents);
    bool Append(const wstring & aContents);
    LONGLONG GetSize();
    void Close();

  private:
    wstring      mFilePath;
    bool         mExists;
    CharsetEncoding::Type mEncoding;
    AutoHandle   mFileHandle;

    bool WriteContents(const wstring & aContents, bool aAppend);
  }; // File
}

#endif // _FILE_H_
