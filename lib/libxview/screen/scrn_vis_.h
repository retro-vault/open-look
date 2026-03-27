/*
 * Declares internal interfaces used by the XView screen module.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(SCRN_VIS__H)
#define SCRN_VIS__H

#include <xview/pkg.h>
#include <xview_private/scrn_impl.h>

Pkg_private Screen_visual *screen_get_visual(Display *display, Screen_info *screen, XVisualInfo *visual_info);
Pkg_private Screen_visual *screen_get_image_visual(Display *display, Screen_info *screen, XID xid, unsigned int depth);
Pkg_private Screen_visual *screen_new_visual(Display *display, Screen_info *screen, XID xid, unsigned int depth, XVisualInfo *visual_info);

#endif
