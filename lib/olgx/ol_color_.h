/*
 * ol_color_.h: declarations and shared types for the ol_color_ module.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */

#if !defined(OL_COLOR__H)
#define OL_COLOR__H

#include <olgx_impl.h>

/* hsv_to_rgb: Convert hsv to rgb. */
void hsv_to_rgb(HSV *hsv, RGB *rgb);
/* rgb_to_hsv: Convert rgb to hsv. */
void rgb_to_hsv(RGB *rgb, HSV *hsv);
/* rgb_to_xcolor: Convert rgb to xcolor. */
void rgb_to_xcolor(RGB *r, XColor *x);
/* hsv_to_xcolor: Convert hsv to xcolor. */
void hsv_to_xcolor(HSV *h, XColor *x);
/* xcolor_to_hsv: Convert xcolor to hsv. */
void xcolor_to_hsv(XColor *x, HSV *h);
/* olgx_hsv_to_3D: Convert hsv to 3 d. */
void olgx_hsv_to_3D(HSV *bg1, XColor *bg2, XColor *bg3, XColor *white);
/* olgx_calculate_3Dcolors: Process calculate 3 dcolors. */
void olgx_calculate_3Dcolors(XColor *fg, XColor *bg1, XColor *bg2, XColor *bg3, XColor *white);

#endif
