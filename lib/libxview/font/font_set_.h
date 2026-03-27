/*
 * Declares internal interfaces used by the XView font module.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(FONT_SET__H)
#define FONT_SET__H

#include <xview/pkg.h>
#include <xview/font.h>

Pkg_private Xv_opaque font_set_avlist(Xv_Font font_public, Attr_attribute avlist[]);

#endif
