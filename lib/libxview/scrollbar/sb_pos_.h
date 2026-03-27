/*
 * Declares internal interfaces used by the XView scrollbar module.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(SB_POS__H)
#define SB_POS__H

#include <xview/pkg.h>
#include <xview_private/sb_impl.h>

Pkg_private void sb_resize(Xv_scrollbar_info *sb);
Pkg_private void scrollbar_init_positions(Xv_scrollbar_info *sb);
Pkg_private void scrollbar_position_elevator(Xv_scrollbar_info *sb, int paint, Scroll_motion motion);
Pkg_private void scrollbar_absolute_position_elevator(Xv_scrollbar_info *sb, int pos);
Pkg_private void scrollbar_top_anchor_rect(Xv_scrollbar_info *sb, Rect *r);
Pkg_private void scrollbar_bottom_anchor_rect(Xv_scrollbar_info *sb, Rect *r);
Pkg_private void sb_normalize_rect(Xv_scrollbar_info *sb, Rect *r);
Pkg_private int scrollbar_available_cable(Xv_scrollbar_info *sb);
Pkg_private void sb_minimum(Xv_scrollbar_info *sb);
Pkg_private void sb_abbreviated(Xv_scrollbar_info *sb);
Pkg_private void sb_full_size(Xv_scrollbar_info *sb);
Pkg_private int sb_margin(Xv_scrollbar_info *sb);
Pkg_private int sb_marker_height(Xv_scrollbar_info *sb);
Pkg_private void sb_rescale(Xv_scrollbar_info *sb, Frame_rescale_state scale);
Pkg_private void scrollbar_line_backward_rect(Xv_scrollbar_info *sb, Rect *r);
Pkg_private void scrollbar_absolute_rect(Xv_scrollbar_info *sb, Rect *r);
Pkg_private void scrollbar_line_forward_rect(Xv_scrollbar_info *sb,  Rect *r);
Xv_private int scrollbar_width_for_scale(Frame_rescale_state scale);
Pkg_private int sb_elevator_height(Xv_scrollbar_info *sb, Scrollbar_size size);
Xv_private int scrollbar_minimum_size(Scrollbar sb_public);

#endif
