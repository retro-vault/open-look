/*
 * ol_misc_.h: declarations and shared types for the ol_misc_ module.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */

#if !defined(OL_MISC__H)
#define OL_MISC__H

#include <olgx_impl.h>

/* olgx_draw_resize_corner: Draw resize corner. */
void olgx_draw_resize_corner(Graphics_info *info, Window win, int x, int y, int type, int state);
/* olgx_draw_pushpin: Draw pushpin. */
void olgx_draw_pushpin(Graphics_info *info, Window win, int x, int y, int type);
/* olgx_draw_check_box: Draw check box. */
void olgx_draw_check_box(Graphics_info *info, Window win, int x, int y, int state);
/* olgx_draw_text_ledge: Draw text ledge. */
void olgx_draw_text_ledge(Graphics_info *info, Window win, int x, int y, int width);

#endif
