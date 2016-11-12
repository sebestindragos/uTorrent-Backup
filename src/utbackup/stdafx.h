#pragma once

// Change these values to use different versions
// #define WINVER		0x0500
// #define _WIN32_WINNT	0x0501
// #define _WIN32_IE	0x0501
#define _RICHEDIT_VER	0x0200

#define _STDAFX_INCLUDED_

#define _CRT_SECURE_NO_WARNINGS

#pragma warning(push, 0) 

#include <Windows.h>
#include <tchar.h>
#include <commctrl.h>

#include <atlstr.h>
#include <atlbase.h>
#include "third-party/wtl/include/atlapp.h"

extern CAppModule _Module;

#define _WTL_NO_CSTRING

#include <atlwin.h>
#include "third-party/wtl/include/atlcrack.h"
#include "third-party/wtl/include/atlframe.h"
#include "third-party/wtl/include/atlctrls.h"
#include "third-party/wtl/include/atlctrlx.h"
#include "third-party/wtl/include/atlmisc.h"
#include "third-party/wtl/include/atlddx.h"
#include "third-party/wtl/include/atldlgs.h"

#pragma warning (pop)

#include "utbackup/resource.h"

//STL
#include <string>
#include <unordered_map>
#include <vector>
#include <stack>
#include <deque>
#include <queue>
#include <functional>
#include <algorithm>
#include <memory>

using namespace std;

// utils
#include "common/util/WinException.h"
#include "common/util/ExceptionUtil.h"
#include "common/util/FileUtil.h"
#include "common/util/FilePath.h"
#include "common/os/WinVersion.h"
#include "common/util/StringUtil.h"

using namespace FileUtil;

#include "VersionDef.h"
#include "Globals.h"

#if defined _M_IX86
  #pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_IA64
  #pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='ia64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
  #pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
  #pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
