/*
 * Declares internal interfaces used by the XView openwin module.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(OW_VIEW__H)
#define OW_VIEW__H

#include <xview/pkg.h>
#include <xview_private/ow_impl.h>

Pkg_private void openwin_create_initial_view(Xv_openwin_info *owin);
Pkg_private void openwin_destroy_views(Xv_openwin_info *owin);
Pkg_private int openwin_count_views(Xv_openwin_info *owin);
Pkg_private Openwin_view_info *openwin_nth_view(Xv_openwin_info *owin, int place);
Pkg_private int openwin_viewdata_for_view(Xv_Window window, Openwin_view_info **view);
Pkg_private int openwin_viewdata_for_sb(Xv_openwin_info *owin, Scrollbar sb, Openwin_view_info **view, Scrollbar_setting *sb_direction, int *last_sb);
Pkg_private void openwin_split_view(Xv_openwin_info *owin, Openwin_view_info *view, Openwin_split_direction direction, int pos, int view_start);
Pkg_private int openwin_fill_view_gap(Xv_openwin_info *owin, Openwin_view_info *view);
Pkg_private void openwin_copy_scrollbar(Xv_openwin_info *owin, Scrollbar sb, Openwin_view_info *to_view);
Pkg_private void openwin_remove_split(Xv_openwin_info *owin, Openwin_view_info *);

#endif
