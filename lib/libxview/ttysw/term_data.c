/*
 * Implements the term data routines used by the XView ttysw module.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#include <sys/types.h>
#include <xview_private/termsw_.h>

Xv_pkg          xv_termsw_pkg = {
    "Termsw",
    (Attr_pkg) ATTR_PKG_TERMSW,
    sizeof(Xv_termsw),
    &xv_openwin_pkg,
    termsw_folio_init,
    termsw_folio_set,
    termsw_folio_get,
    termsw_folio_destroy,
    NULL			/* no find proc */
};

Xv_pkg          xv_termsw_view_pkg = {
    "Termsw_view",
    (Attr_pkg) ATTR_PKG_TERMSW_VIEW,
    sizeof(Xv_termsw_view),
    &xv_window_pkg,
    termsw_view_init,
    termsw_view_set,
    termsw_view_get,
    termsw_view_destroy,
    NULL			/* no find proc */
};
