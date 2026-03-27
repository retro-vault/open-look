/*
 * Declares private XView interfaces, types, and macros for window.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(WINDOW__H)
#define WINDOW__H

#include <xview/pkg.h>
#include <xview/window.h>
#include <xview_private/windowimpl.h>
#include <X11/Xresource.h>

Pkg_private int window_init(Xv_Window parent_public, Xv_Window win_public, Attr_avlist avlist);
Pkg_private XID window_new(Display *display, Xv_opaque screen, Window_info *win,  int cmap_id, Xv_Drawable_info *parent_info);
Xv_private void window_set_bit_gravity(Xv_Window win_public, int value);
void xv_main_loop(Xv_Window win_public);
int window_done(Xv_Window win);
Pkg_private int window_get_parent_dying(void);
Pkg_private int window_set_parent_dying(void);
Pkg_private int window_unset_parent_dying(void);
Pkg_private int window_destroy_win_struct(register Xv_Window win_public, Destroy_status status);


#endif
