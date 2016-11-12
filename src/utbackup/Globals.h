#ifndef _GLOBALS_H_
#define _GLOBALS_H_

/**
 * Namespace used for storring global constants.
 *
 * @author Dragos Sebestin
 */
namespace Globals
{
  // uTorrent Backup reg entries
  const wstring kUtbRegSettingsKey = L"Software\\uTorrent Backup\\Settings";

  // uTorrent files
  const wstring kUtSettingsFile = L"settings.dat";
  const wstring kUtResumeFile = L"resume.dat";
}

#endif // _GLOBALS_H_
