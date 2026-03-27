/*
 * ol_button_.h: declarations and shared types for the ol_button_
 * module.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */

#if !defined(OL_BUTTON__H)
#define OL_BUTTON__H

#include <olgx/olgx.h>
#include <olgx_impl.h>

/* olgx_draw_button: Draw a full button, including label, marks, and state effects. */
void olgx_draw_button(Graphics_info *info, Window win, int x, int y, int width, int height, void *label, int state);
/* olgx_draw_varheight_button: Draw a variable-height button frame with 2D/3D state styling. */
void olgx_draw_varheight_button(Graphics_info *info, Window win, int x, int y, int width, int height, int state);
/* olgx_draw_menu_mark: Draw vertical/horizontal menu arrows with optional interior fill. */
void olgx_draw_menu_mark(Graphics_info *info, Window win, int x, int y, int state, int fill_in);
/* olgx_draw_abbrev_button: Draw an abbreviated menu button with a centered menu mark. */
void olgx_draw_abbrev_button(Graphics_info *info, Window win, int x, int y, int state);
/* olgx_stipple_rect: Stipple a rectangle to render inactive/disabled appearance. */
void olgx_stipple_rect(Graphics_info *info, Window win, int x, int y, int width, int height);
/* olgx_draw_text: Draw and clip text labels, including inverse/inactive/arrow states. */
void olgx_draw_text(Graphics_info *info, Window win, char *string, int x, int y, int max_width, int state);
#ifdef OW_I18N
/* olgx_draw_text: Draw and clip text labels, including inverse/inactive/arrow states. */
void olgx_draw_text(Graphics_info *info, Window win, void *string, int x, int y, int max_width, int state);
#else
/* olgx_draw_text: Draw and clip text labels, including inverse/inactive/arrow states. */
void olgx_draw_text(Graphics_info *info, Window win, char *string, int x, int y, int max_width, int state);
#endif
/* olgx_draw_pixmap_label: Draw a pixmap label, handling invoked-state color inversion. */
void olgx_draw_pixmap_label(Graphics_info *info, Window win, Pixmap pix, int x, int y, int width, int height, int state);
/* olgx_draw_ximage_label: Draw an XImage label, handling invoked-state color inversion. */
void olgx_draw_ximage_label(Graphics_info *info, Window win, XImage *xim, int x, int y, int width, int height, int state);
/* olgx_draw_textscroll_button: Draw a single-direction text-scroll control button. */
void olgx_draw_textscroll_button(Graphics_info *info, Window win, int x, int y, int state);
/* olgx_draw_numscroll_button: Draw a two-arrow numeric scroll control with per-side states. */
void olgx_draw_numscroll_button(Graphics_info *info, Window win, int x, int y, int state);
/* olgx_draw_accel_label: Draw menu text with qualifier/mark/key accelerator columns. */
void olgx_draw_accel_label(Graphics_info *info, Window win, int x, int y, int width, int height, void *main_label, int m_pos, void *qualifier_label, int q_pos, int mark_type, int mark_pos, void *key_label, int key_pos, void *background_pixmap, int state);
/* olgx_draw_accel_button: Draw a button plus accelerator layout and inactive stippling. */
void olgx_draw_accel_button(Graphics_info *info, Window win, int x, int y, int width, int height, void *main_label, int m_pos, void *qualifier_label, int q_pos, int mark_type, int mark_pos, void *key_label, int key_pos, void *background_pixmap, int state);
/* olgx_draw_accel_choice_item: Draw a choice-item frame with accelerator-style label content. */
void olgx_draw_accel_choice_item(Graphics_info *info, Window win, int x, int y, int width, int height, void *main_label, int m_pos, void *qualifier_label, int q_pos, int mark_type, int mark_pos, void *key_label, int key_pos, void *background_pixmap, int state);

#endif
