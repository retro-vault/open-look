/*
 * Declares public XView interfaces, types, and attributes for pw line.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(PW_LINE__H)
#define PW_LINE__H

#include <xview/pkg.h>
#include <xview_private/pw_impl.h>

Xv_public void pw_line(register struct pixwin *pw, register int x0, register int y0, register int x1, register int y1, struct pr_brush *brush, Pr_texture *tex, int op);
Pkg_private char *pw_short_to_char(short *pattern, int *len);
Xv_private void xv_draw_rectangle(Xv_opaque pw, int x, int y, int w, int h, int linestyle, int op);

#endif
