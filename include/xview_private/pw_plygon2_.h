/*
 * Declares private XView interfaces, types, and macros for pw plygon2.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(PW_PLYGON2__H)
#define PW_PLYGON2__H

#include <xview/pkg.h>
#include <xview_private/pw_impl.h>

Xv_public int pw_polygon_2(register struct pixwin *pw, int dx, int dy, int nbds, int npts[], struct pr_pos *vlist, int op, struct pixrect *spr, int sx, int sy);

#endif
