/*
 * ol_slider_.h: declarations and shared types for the ol_slider_
 * module.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */

#if !defined(OL_SLIDER__H)
#define OL_SLIDER__H

#include <olgx_impl.h>

/* olgx_draw_slider: Draw slider. */
void olgx_draw_slider(Graphics_info *info, Window win, int x, int y, int width, int oldval, int newval, int state);
/* olgx_draw_horizontal_slider: Draw horizontal slider. */
void olgx_draw_horizontal_slider(Graphics_info *info, Window win, int x, int y, int width, int value, int state);
/* olgx_draw_slider_control: Draw slider control. */
void olgx_draw_slider_control(Graphics_info *info, Window win, int x, int y, int state);
/* olgx_update_horizontal_slider: Set horizontal slider. */
void olgx_update_horizontal_slider(Graphics_info *info, Window win, int x, int y, int width, int old_value, int new_value, int state);
/* olgx_update_vertical_slider: Set vertical slider. */
void olgx_update_vertical_slider(Graphics_info *info, Window win, int x, int y, int height, int old_value, int new_value, int state);
/* olgx_draw_vertical_slider: Draw vertical slider. */
void olgx_draw_vertical_slider(Graphics_info *info, Window win, int x, int y, int height, int value, int state);
/* olgx_draw_gauge: Draw gauge. */
void olgx_draw_gauge(Graphics_info *info, Window win, int x, int y, int width, int oldval, int newval, int state);
/* olgx_draw_horiz_gauge: Draw horiz gauge. */
void olgx_draw_horiz_gauge(Graphics_info *info, Window win, int x, int y, int width, int value);
/* olgx_update_horiz_gauge: Set horiz gauge. */
void olgx_update_horiz_gauge(Graphics_info *info, Window win, int x, int y, int oldval, int newval);
/* olgx_draw_vertical_gauge: Draw vertical gauge. */
void olgx_draw_vertical_gauge(Graphics_info *info, Window win, int x, int y, int width, int value);
/* olgx_update_vertical_gauge: Set vertical gauge. */
void olgx_update_vertical_gauge(Graphics_info *info, Window win, int x, int y, int width, int oldval, int newval);

#endif
