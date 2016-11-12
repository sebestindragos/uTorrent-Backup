#ifndef _UTORRENT_BACKUP_H_
#define _UTORRENT_BACKUP_H_

#include "propertypool/PropertyPool.h"
#include "common/util/AutoHandle.h"

class bEncodeManager;

/**
 * Class representing the application entity.
 *
 * @author: Dragos Sebestin
 */
class uTorrentBackup
{
public:

  /**
   * Class constructors and desctructor.
   */
  uTorrentBackup(CWindow aFrameWnd = NULL);
  ~uTorrentBackup();

  /**
   * Method for retrieving the global property pool.
   */
  IPropertyPool & GetPropertyPool();

  /**
   * Backup operation. Handles paths based on the property pool.
   */
  bool Backup();

  /**
   * Restore operation. Handles paths based on the property pool.
   */
  bool Restore(bool aWaitForOperation = false);

  /**
   * Check if an exception ocurred in the worker thread.
   */
  bool HasException();

  /**
   * Pass the worket thread exception.
   */
  void RaiseException();

private:

  PropertyPool    mPropertyPool;
  CWindow         mFrameWnd;

  function<bool()>     mWorkerFunc;
  bool                 mWorkerHadException;
  AutoHandle           mThreadHandle;
  Exception            mThreadException;

  bool DoBackup();
  bool DoCopyFiles(const FilePath & aSourcePath, const FilePath & aDestPath);
  bool DoBackupUnpacked(const FilePath & aUtPath, const FilePath & aBackupPath, 
    const FilePath & aAltPath, const FilePath & aAltBackupPath);
  bool DoBackupPacked(const FilePath & aUtPath, const FilePath & aAltBackupPath, bEncodeManager & aBManager);

  bool DoRestore();
  void DoRestoreUnpacked(const FilePath & aUtPath, const FilePath & aBackupPath, 
    const FilePath & aAltPath, bEncodeManager & aBManager);
  void DoRestorePacked(const FilePath & aUtPath, const FilePath & aAltRestorePath,
    bEncodeManager & aBManager);

  void ReadPersistentProps();
  void SavePersistentProps();

  void ComputeDynamicProps();

  static DWORD WINAPI WorkerThreadProc(LPVOID aData);

  void StartWorkerThread();
};


#endif // _UTORRENT_BACKUP_H_