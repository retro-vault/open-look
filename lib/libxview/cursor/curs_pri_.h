/*
 * Declares internal interfaces used by the XView cursor module.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(CURS_PRI__H)
#define CURS_PRI__H

#include <xview/pkg.h>
#include <X11/Xresource.h>
#include <xview/cursor.h>
#include <xview_private/draw_impl.h>

Pkg_private void cursor_free_x(Xv_Drawable_info *info, Cursor old_cursor);
Pkg_private long unsigned cursor_make_x(Xv_Drawable_info *root_info, int w, int h, int d, int op, int xhot, int yhot, XColor *xfg, XColor *xbg, Xv_opaque pr);
Pkg_private unsigned long cursor_make_x_font(Xv_Drawable_info *root_info, unsigned int src_char, unsigned int mask_char, XColor *xfg, XColor *xbg);
Pkg_private void cursor_set_cursor_internal(Xv_Drawable_info *info, Cursor cursor);

#endif
