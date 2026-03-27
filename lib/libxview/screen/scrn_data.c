/*
 * Implements the scrn data routines used by the XView screen module.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#include <xview_private/screen_.h>
#include <xview_private/scrn_get_.h>

Xv_pkg          xv_screen_pkg = {
    "Screen", ATTR_PKG_SCREEN,
    sizeof(Xv_screen_struct),
    &xv_generic_pkg,
    screen_init,
    screen_set_avlist,
    screen_get_attr,
    screen_destroy,
    NULL			/* no find proc */
};
