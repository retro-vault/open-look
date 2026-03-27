/*
 * Declares internal interfaces used by the XView frame module.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(FM_LAYOUT__H)
#define FM_LAYOUT__H

#include <xview/pkg.h>
#include <xview/frame.h>


Pkg_private int frame_layout(Frame frame_public, register Xv_Window child, Window_layout_op op, unsigned long d1, unsigned long d2, unsigned long d3, unsigned long d4, unsigned long d5);
Pkg_private int frame_adjust_rect(Frame frame_public, Xv_Window child, int is_subframe, Rect *r);
Pkg_private Xv_Window frame_last_child(Xv_Window first);
Xv_private void frame_grant_extend_to_edge(Frame frame_public, register int to_right);

#endif
