#ifndef __JAWT_INFO_H__
#define __JAWT_INFO_H__
#include <jawt.h>
#include <jawt_md.h>

struct JAWTInfo {
    JAWT awt;
    JAWT_DrawingSurface* ds;
    JAWT_DrawingSurfaceInfo* dsi;
#ifndef linux
    JAWT_Win32DrawingSurfaceInfo * dsi_win;
#endif
#ifdef linux
    JAWT_X11DrawingSurfaceInfo * dsi_win;
#endif
};

#endif //__JAWT_INFO_H__
