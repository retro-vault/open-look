/*
 * Declares public XView interfaces, types, and attributes for win
 * geom.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(WIN_GEOM__H)
#define WIN_GEOM__H

#include <xview/pkg.h>
#include <xview/win_struct.h>
#include <xview/window.h>
#include <xview_private/draw_impl.h>
#include <xview_private/windowimpl.h>
#include <xview/win.h>


Xv_private void win_x_getrect(Display *display, XID xid, register Rect *rect);
Xv_private void win_getrect(Xv_object window, Rect *rect);
Xv_private void win_setrect(Xv_object window, Rect *rect);
Xv_private void win_set_outer_rect(Xv_object window, Rect *rect);
Xv_private void win_get_outer_rect(Xv_object window, Rect *rect);
Xv_private void win_getsize(Xv_object window, Rect *rect);
Xv_private int win_get_retained(Xv_object window);
Xv_private int win_translate_xy(Xv_object src, Xv_object dst, int src_x, int src_y, int *dst_x, int *dst_y);
Xv_private int win_translate_xy_internal(Display *display, XID src_id, XID dst_id, int src_x, int src_y, int *dst_x, int *dst_y);
Xv_private XID win_pointer_under(Xv_opaque window, int x, int y);

#endif
