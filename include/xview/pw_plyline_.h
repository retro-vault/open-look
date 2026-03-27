/*
 * Declares public XView interfaces, types, and attributes for pw
 * plyline.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(PW_PLYLINE__H)
#define PW_PLYLINE__H

#include <xview/pkg.h>
#include <xview_private/pw_impl.h>

Xv_public int pw_polyline(Xv_opaque pw, int dx, int dy, int npts, struct pr_pos *ptlist, u_char *mvlist, struct pr_brush *brush, Pr_texture *tex, int op);

#endif
