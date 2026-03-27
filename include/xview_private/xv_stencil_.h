/*
 * Declares private XView interfaces, types, and macros for xv stencil.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(XV_STENCIL__H)
#define XV_STENCIL__H

#include <xview/pkg.h>
#include <xview_private/pw_impl.h>

Xv_private int xv_stencil_internal(Display *display, Xv_Drawable_info *info, Drawable d, GC gc, int dx, int dy, int width, int height, Xv_opaque stpr, int stx, int sty, Xv_opaque spr, int sx, int sy, Xv_Drawable_info *dest_info);
Xv_public int xv_stencil(Xv_opaque window, int dx, int dy, int width, int height, int op, struct pixrect *stpr, int stx, int sty, struct pixrect *spr, int sx, int sy);

#endif
