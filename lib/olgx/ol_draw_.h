/*
 * ol_draw_.h: declarations and shared types for the ol_draw_ module.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */

#if !defined(OL_DRAW__H)
#define OL_DRAW__H

#include <olgx_impl.h>

/* calc_add_ins: Add calc ins. */
int calc_add_ins(int width, short add_ins[STRING_SIZE]);
/* olgx_draw_box: Draw box. */
void olgx_draw_box(Graphics_info *info, Window win, int x, int y, int width, int height, int state, Bool fill_in);
/* olgx_draw_choice_item: Draw choice item. */
void olgx_draw_choice_item(Graphics_info *info, Window win, int x, int y, int width, int height, void *label, int state);
/* olgx_draw_drop_target: Draw drop target. */
void olgx_draw_drop_target(Graphics_info *info, Window win, void *label, int x, int y, int state);

#endif
