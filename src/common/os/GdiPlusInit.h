#ifndef _GDI_PLUS_INIT_H_
#define _GDI_PLUS_INIT_H_

#include <GdiPlus.h>

using namespace Gdiplus;

/**
 * Class used for GDI+ init and cleanup.
 *
 * @author Dragos Sebestin
 */
class GdiPlusInit
{
public:

  static bool Initialize()
  {
    static GdiPlusInit init;
    return true;
  }

private:

  GdiplusStartupInput gdiplusStartupInput;
  ULONG_PTR           gdiplusToken;

  /**
   * Class c-tors/d-tor
   */
  GdiPlusInit()
  {
    // Initialize GDI+.
    ::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
  }

  ~GdiPlusInit()
  {
    ::GdiplusShutdown(gdiplusToken);
  }

  GdiPlusInit(const GdiPlusInit &) {}
};

#endif // _GDI_PLUS_INIT_H_
