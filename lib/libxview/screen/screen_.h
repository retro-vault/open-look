/*
 * Declares internal interfaces used by the XView screen module.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(SCREEN__H)
#define SCREEN__H

#include <xview/pkg.h>
#include <xview/screen.h>
#include <xview/window.h>
#include <xview/xv_xrect.h>
#include <X11/Xresource.h>

Pkg_private int screen_init(Xv_opaque parent, Xv_Screen screen_public, Attr_avlist avlist);
Pkg_private int screen_destroy(Xv_Screen screen_public, Destroy_status status);
Pkg_private Xv_opaque screen_set_avlist(Xv_Screen screen_public, Attr_attribute avlist[]);
Xv_private void screen_set_clip_rects(Xv_Screen screen_public, XRectangle *xrect_array, int rect_count);
Xv_private Xv_xrectlist *screen_get_clip_rects(Xv_Screen screen_public);
Xv_private void screen_set_cached_window_busy(Xv_Screen screen_public, Xv_window window, int busy);
Xv_private int screen_get_sun_wm_protocols(Xv_Screen screen_public);
Xv_private int screen_check_sun_wm_protocols(Xv_Screen screen_public, Atom check_atom);

#endif
