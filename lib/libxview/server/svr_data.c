/*
 * Implements the svr data routines used by the XView server module.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#include <xview_private/server_.h>
#include <xview_private/svr_get_.h>
#include <xview_private/svr_set_.h>

Xv_pkg          xv_server_pkg = {
    "Server",
    ATTR_PKG_SERVER,
    sizeof(Xv_server_struct),
    &xv_generic_pkg,
    server_init,
    server_set_avlist,
    server_get_attr,
    server_destroy,
    NULL			/* no find proc */
};
