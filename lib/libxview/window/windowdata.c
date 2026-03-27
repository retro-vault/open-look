/*
 * Implements the windowdata routines used by the XView window module.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#include <xview_private/window_.h>
#include <xview_private/window_get_.h>
#include <xview_private/window_set_.h>

Xv_pkg          xv_window_pkg = {
    "Window", ATTR_PKG_WIN,
    sizeof(Xv_window_struct),
    &xv_drawable_pkg,
    window_init,
    window_set_avlist,
    window_get_attr,
    window_destroy_win_struct,
    NULL
};
