/*
 * Declares public XView interfaces, types, and attributes for font
 * get.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(FONT_GET__H)
#define FONT_GET__H

#include <xview/pkg.h>
#include <xview/font.h>

Pkg_private Xv_opaque font_get_attr(Xv_font_struct *font_public, int *status, Font_attribute attr, va_list args);

#endif
