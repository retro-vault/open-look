/*
 * Declares public XView interfaces, types, and attributes for pw put.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(PW_PUT__H)
#define PW_PUT__H

#include <xview/pkg.h>
#include <xview_private/pw_impl.h>

Xv_public int pw_put(Xv_opaque pw, int x, int y, int val);

#endif
