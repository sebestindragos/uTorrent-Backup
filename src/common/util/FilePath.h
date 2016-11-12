#ifndef _FILE_PATH_H_
#define _FILE_PATH_H_

namespace FileUtil
{

  /**
  * Class used for correctly manipulating a file path.
  */
  class FilePath
  {
  public:

    /**
     * Class constructors.
     */
    FilePath();

    FilePath(const FilePath & aOther);
    FilePath(FilePath && aOther);

    FilePath(const wstring & aPath);

    FilePath & operator = (FilePath aOther);

    FilePath & Append(const wstring & aPath);
        
    FilePath operator + (const FilePath & aOtherPath);

    const wstring & Get() const;

    FilePath GetParent() const;

    void ReplaceExtension(const wstring & aNewExtension);

    void EnsureEndingBackslash();

    wstring GetName();

    wstring GetExtension();

    bool IsAbsolute() const;
      
  private:

    wstring mPath;

    void ExtractValidPath(const wstring & aPath);
  }; // class FilePath

}; // FileUtil

#endif // _FILE_PATH_H_