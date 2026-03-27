/*
 * Declares internal interfaces used by the XView pw module.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(PW_PLYPT__H)
#define PW_PLYPT__H

#include <xview/pkg.h>
#include <xview_private/pw_impl.h>

Xv_public int pw_polypoint(register Xv_opaque pw, register int dx, register int dy, register int npts, register struct pr_pos *ptlist, register int op);

#endif
