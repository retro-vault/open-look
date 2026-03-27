/*
 * Implements the fs data routines used by the XView fullscreen module.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#include <xview_private/fs_.h>
#include <xview_private/fs_get_.h>
#include <xview_private/fs_set_.h>

Xv_pkg          xv_fullscreen_pkg = {
    "Fullscreen", ATTR_PKG_FULLSCREEN,
    sizeof(Xv_fullscreen),
    &xv_generic_pkg,
    fullscreen_init_internal,
    fullscreen_set_avlist,
    fullscreen_get_attr,
    fullscreen_destroy_internal,
    NULL			/* no find proc */
};

/*
 * When not zero will not actually acquire exclusive io lock so that the
 * debugger doesn't get hung.
 */
int             fullscreendebug;

/*
 * When not zero will not grab device server/pointer/keyboard
 */
int             fullscreendebugserver;
int             fullscreendebugptr;
int             fullscreendebugkbd;
