#ifndef _IPROPERY_POOL_H_
#define _IPROPERY_POOL_H_

class IPropertyObserver;

/**
 * Property pool interface class.
 * Views will hold a ref to this interface to query for global property values.
 */
class IPropertyPool
{
public:

  /**
   * Retrieve a property by name.
   */
  virtual const wstring & GetProperty(const wstring & aName) = 0;

  /**
   * Set a property
   */
  virtual void SetProperty(const wstring & aName, const wstring & aValue) = 0;

  /**
   * Add property observer
   */
  virtual void AddObserver(IPropertyObserver * aObserver) = 0;
};

// Common Properties
#define PROP_TRUE                          L"true"
#define PROP_FALSE                         L"false"

#define UT_PATH_PROP                       L"uTorrent.path"
#define UT_ALTERNATEFOLDER_PROP            L"uTorrent.alternatefolder"
#define UT_ALTERNATEFOLDERPATH_PROP        L"uTorrent.alternatefolderpath"

#define UTB_BACKUPPATH_PROP                L"uTorrentBackup.backuppath"
#define UTB_BACKUPALTERNATE_PROP           L"uTorrentBackup.backupalternate"
#define UTB_BACKUPALTERNATEPATH_PROP       L"uTorrentBackup.backupalternatepath"
#define UTB_BACKUPPACKFILES_PROP           L"uTorrentBackup.backuppackfiles"

#define UTB_CREATE_VERSION                 L"uTorrentBackup.createversion"

#endif // _IPROPERY_POOL_H_