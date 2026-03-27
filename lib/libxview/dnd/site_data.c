/*
 * Implements the site data routines used by the XView dnd module.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#include <xview_private/site_pblc_.h>

Xv_pkg		xv_drop_site_item = {
    "DropSite", ATTR_PKG_DND,
    sizeof(Xv_drop_site_struct),
    &xv_generic_pkg,
    dnd_site_init,
    dnd_site_set_avlist,
    dnd_site_get_attr,
    dnd_site_destroy,
    NULL
};
