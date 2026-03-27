/*
 * ol_sb_.h: declarations and shared types for the ol_sb_ module.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */

#if !defined(OL_SB__H)
#define OL_SB__H

#include <olgx_impl.h>

/* olgx_draw_scrollbar: Draw scrollbar. */
void olgx_draw_scrollbar(Graphics_info *ginfo, Window win, int x, int y, int length, int elev_pos, int old_elev_pos, int prop_pos, int prop_length, int state);
/* olgx_draw_elevator: Draw elevator. */
void olgx_draw_elevator(Graphics_info *info, Window win, int x, int y, int state);
/* olgx_scroll_stipple_rects: Process scroll stipple rects. */
void olgx_scroll_stipple_rects(Graphics_info *info, Window win, XRectangle *rects, int numrects);
XRectangle *olgx_compute_intersection(XRectangle *rect1, XRectangle *rect2);

#endif
