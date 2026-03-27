/*
 * Declares public XView interfaces, types, and attributes for xv
 * error.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(XV_ERROR__H)
#define XV_ERROR__H

#include <xview/pkg.h>

Xv_public char *xv_error_format(Xv_object object, Attr_avlist avlist);
Xv_public int xv_error_default(Xv_object object, Attr_avlist avlist);
Xv_public int _xv_error(Xv_object object, ...);

#endif
