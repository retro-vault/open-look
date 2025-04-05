#if !defined(OL_COLOR__H)
#define OL_COLOR__H

#include <olgx_impl.h>

void hsv_to_rgb(HSV *hsv, RGB *rgb);
void rgb_to_hsv(RGB *rgb, HSV *hsv);
void rgb_to_xcolor(RGB *r, XColor *x);
void hsv_to_xcolor(HSV *h, XColor *x);
void xcolor_to_hsv(XColor *x, HSV *h);
void olgx_hsv_to_3D(HSV *bg1, XColor *bg2, XColor *bg3, XColor *white);
void olgx_calculate_3Dcolors(XColor *fg, XColor *bg1, XColor *bg2, XColor *bg3, XColor *white);

#endif
