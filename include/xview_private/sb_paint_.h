/*
 * Declares private XView interfaces, types, and macros for sb paint.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(SB_PAINT__H)
#define SB_PAINT__H

#include <xview/pkg.h>
#include <xview_private/sb_impl.h>

Xv_public void scrollbar_paint(Scrollbar sb_public);
Pkg_private void scrollbar_paint_elevator(Xv_scrollbar_info *sb);
Pkg_private void scrollbar_paint_elevator_move(Xv_scrollbar_info *sb, int new_pos);
Pkg_private void scrollbar_invert_region(Xv_scrollbar_info *sb, Scroll_motion motion);

static void scrollbar_paint_anchor(Xv_scrollbar_info *sb, Rect *r, int invoked);
static void scrollbar_proportional_indicator(Xv_scrollbar_info *sb, int elev_pos, int *position, int *length);

#endif
