/*
 * Implements the ow data routines used by the XView openwin module.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#include <xview_private/openwin_.h>
#include <xview_private/ow_get_.h>
#include <xview_private/ow_set_.h>

Xv_pkg          xv_openwin_pkg = {
    "Open Window",		/* seal -> package name */
    (Attr_pkg) ATTR_PKG_OPENWIN,/* openwin attr */
    sizeof(Xv_openwin),		/* size of the openwin data struct */
    &xv_window_pkg,		/* pointer to parent */
    openwin_init,		/* init routine for openwin */
    openwin_set,		/* set routine */
    openwin_get,		/* get routine */
    openwin_destroy,		/* destroy routine */
    NULL			/* No find proc */
};
