/*
 * Declares internal interfaces used by the XView openwin module.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(OW_RESIZE__H)
#define OW_RESIZE__H

#include <xview/pkg.h>
#include <xview_private/ow_impl.h>

Pkg_private int openwin_adjust_views(Xv_openwin_info *owin, Rect *owin_rect);
Pkg_private void openwin_adjust_view(Xv_openwin_info *owin, Openwin_view_info *view, Rect *view_rect);
Pkg_private void openwin_place_scrollbar(Xv_object owin_public, Xv_opaque view_public, Scrollbar sb, Scrollbar_setting direction, Rect *r, Rect *sb_r);
Pkg_private int openwin_border_width(Openwin owin_public, Xv_opaque view_public);
Pkg_private void openwin_view_rect_from_avail_rect(Xv_openwin_info *owin, Openwin_view_info *view, Rect *r);

#endif
