/*
 * Declares public XView interfaces, types, and attributes for scrn
 * get.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(SCRN_GET__H)
#define SCRN_GET__H

#include <xview/pkg.h>
#include <xview_private/scrn_impl.h>
#include <X11/Xutil.h>

Pkg_private Xv_opaque screen_get_attr(Xv_Screen screen_public, int *status, Attr_attribute attr, va_list args);
Pkg_private XVisualInfo *screen_match_visual_info(Screen_info *screen, long mask, XVisualInfo *template);
Xv_private Xv_Window screen_get_cached_window(Xv_Screen screen_public, Notify_func event_proc, int borders, Visual *visual, int *new_window);
Xv_private GC *screen_get_cached_gc_list(Screen_info *screen, Xv_Window window);
Xv_private void screen_adjust_gc_color(Xv_Window window, int gc_index);

#endif
