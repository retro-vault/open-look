/*
 * Declares private XView interfaces, types, and macros for font x.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(FONT_X__H)
#define FONT_X__H

#include <xview/pkg.h>
#include <X11/Xresource.h>
#include <pixrect/pixrect.h>

#ifdef OW_I18N
Pkg_private XFontSet xv_load_font_set(Display *dpy, char *locale, char **fs_list);
#endif 
Pkg_private XID xv_load_x_font(register Display *display, char *name, Xv_opaque *font_opaque, int *default_x, int *default_y, int *max_char, int *min_char);
Xv_private void xv_x_char_info(XFontStruct *font, int i, int *x_home, int *y_home, int *x_advance, int *y_advance, Pixrect **pr);
Xv_public void xv_real_baseline_when_using_pf(Xv_opaque font, int ch, int *x_x, int *x_y, int pr_x, int pr_y);
Pkg_private void xv_unload_x_font(register Display *display, Xv_opaque font_opaque);

#endif
