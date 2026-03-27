/*
 * Implements the font data routines used by the XView font module.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#include <xview_private/font_.h>
#include <xview_private/font_set_.h>
#include <xview_private/font_get_.h>

Xv_pkg          xv_font_pkg = {
    "Font", ATTR_PKG_FONT,
    sizeof(Xv_font_struct),
    &xv_generic_pkg,
    font_init,
    font_set_avlist,
    font_get_attr,
    font_destroy_struct,
    font_find_font
};
