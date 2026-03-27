/*
 * Declares internal interfaces used by the XView pw module.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(PW_GET__H)
#define PW_GET__H

#include <xview/pkg.h>
#include <xview_private/pw_impl.h>

Xv_public int pw_get(Xv_opaque drawable, int x, int y);

#endif
