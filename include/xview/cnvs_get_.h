/*
 * Declares public XView interfaces, types, and attributes for cnvs
 * get.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(CNVS_GET__H)
#define CNVS_GET__H

#include <xview/pkg.h>
#include <xview/canvas.h>

Pkg_private Xv_opaque canvas_get_attr(Canvas canvas_public, int *stat, Attr_attribute attr, va_list valist);

#endif
