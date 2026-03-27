/*
 * Declares private XView interfaces, types, and macros for svrim ops.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(SVRIM_OPS__H)
#define SVRIM_OPS__H

#include <xview/pkg.h>
#include <xview_private/svrim_impl.h>

Xv_public int server_image_rop(Xv_opaque dest, int dx, int dy, int dw, int dh, unsigned long op, Xv_opaque src, int sx, int sy);
Xv_public int server_image_stencil(Xv_opaque dest, int dx, int dy, int dw, int dh, unsigned long op, Xv_opaque st, int stx, int sty, Xv_opaque src, int sx, int sy);
Xv_public int server_image_replrop(Xv_opaque dest, int dx, int dy, int dw, int dh, unsigned long op, Xv_opaque src, int sx, int sy);
Xv_public int server_image_vector(Xv_opaque dest, int x0, int y0, int x1, int y1, int op, int value);
Xv_public int server_image_put(Xv_opaque dest, int x, int y, int value);
Xv_public int server_image_get(Xv_opaque dest, int x, int y);
Xv_public Pixrect *server_image_region(Xv_opaque dest, int x, int y, int w, int h);
Xv_public int server_image_colormap(Xv_opaque dest, int index, int count, unsigned char red[], unsigned char green[], unsigned char blue[]);
Xv_public int server_image_pf_text(struct pr_prpos rpr, int op, Pixfont *font, char *string);
#ifdef OW_I18N
Xv_public int server_image_pf_text_wc(struct pr_prpos rpr, int op, Pixfont *font, CHAR *string);
#endif 

#endif
