/*
 * Declares internal interfaces used by the XView pw module.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(PW_VECTOR__H)
#define PW_VECTOR__H

#include <xview/pkg.h>
#include <xview_private/pw_impl.h>

Xv_public int xv_vector(Xv_opaque window, register int x0, register int y0, register int x1, register int y1, int op, int cms_index);

#endif
