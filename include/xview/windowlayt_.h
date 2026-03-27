/*
 * Declares public XView interfaces, types, and attributes for
 * windowlayt.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(WINDOWLAYT__H)
#define WINDOWLAYT__H

#include <xview/pkg.h>
#include <xview_private/windowimpl.h>

Pkg_private int window_layout(Xv_Window parent, register Xv_Window child, Window_layout_op op, Xv_opaque d1, Xv_opaque d2, Xv_opaque d3, Xv_opaque d4,Xv_opaque d5);

#endif
