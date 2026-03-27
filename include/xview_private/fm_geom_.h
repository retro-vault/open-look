/*
 * Declares private XView interfaces, types, and macros for fm geom.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(FM_GEOM__H)
#define FM_GEOM__H

#include <xview/pkg.h>
#include <xview_private/fm_impl.h>


Pkg_private int frame_height_from_lines(int n, int show_label);
Pkg_private int frame_width_from_columns(int x);
Pkg_private void frame_position_sw(Frame_class_info *frame, Xv_Window snsw, Xv_Window sswprevious, int width, int height, Rect *rect);
Pkg_private int frame_footer_height(Frame_rescale_state scale);
Pkg_private int frame_footer_baseline(Frame_rescale_state scale);
Pkg_private int frame_footer_margin(Frame_rescale_state scale);
Pkg_private int frame_inter_footer_gap(Frame_rescale_state scale);
#ifdef OW_I18N
Pkg_private int frame_IMstatus_height(Frame_rescale_state scale);
#endif

#endif
