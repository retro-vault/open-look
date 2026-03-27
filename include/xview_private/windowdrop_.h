/*
 * Declares private XView interfaces, types, and macros for windowdrop.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(WINDOWDROP__H)
#define WINDOWDROP__H

#include <xview/pkg.h>
#include <xview_private/windowimpl.h>

Pkg_private void win_add_drop_item(Window_info *win, Xv_drop_site dropItem);
Pkg_private Xv_opaque win_delete_drop_item(Window_info *win, Xv_drop_site dropItem, Win_drop_site_mode mode);
Pkg_private void win_add_drop_interest(Window_info *win, Xv_drop_site dropItem);
Pkg_private void win_update_dnd_property(Window_info *win);
Xv_private Xv_opaque win_get_top_level(Xv_Window window);

#endif
