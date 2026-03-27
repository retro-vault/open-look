/*
 * Declares public XView interfaces, types, and attributes for xv text.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(XV_TEXT__H)
#define XV_TEXT__H

#include <xview/pkg.h>
#include <xview_private/pw_impl.h>

Xv_public int pw_char(Xv_opaque pw, int xw, int yw, int op, struct pixfont *pixfont, char c);
Xv_public void xv_ttext(Xv_opaque window, register int xbasew, register int ybasew, int op, Xv_opaque pixfont, char *str);
Xv_public void xv_text(Xv_opaque window, register int xbasew, register int ybasew, int op, Xv_opaque pixfont, char *str);
Xv_public int xv_glyph_charXv_opaque (Xv_opaque window, register int x, register int y, int width, int height, Pixfont *pixfont, char c, int color_index);
Xv_public PIXFONT *pw_pfsysopen(void);
Xv_public int pw_pfsysclose(void);

#endif
