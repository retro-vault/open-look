/*
 * Implements the curs data routines used by the XView cursor module.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#include <xview_private/cursor_.h>

Xv_pkg          xv_cursor_pkg = {
    "Cursor",			/* seal -> package name */
    ATTR_PKG_CURSOR,		/* cursor attr */
    sizeof(Xv_cursor_struct),	/* size of the cursor data struct */
    &xv_generic_pkg,		/* pointer to parent */
    cursor_create_internal,	/* init routine for cursor */
    cursor_set_internal,
    cursor_get_internal,
    cursor_destroy_internal,
    NULL			/* no find proc */
};
