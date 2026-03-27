/*
 * Implements the item data routines used by the XView panel module.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#include <xview_private/item_.h>
#include <xview_private/item_get_.h>
#include <xview_private/item_set_.h>

Xv_pkg          xv_panel_item_pkg = {
    "Item", ATTR_PKG_PANEL,
    sizeof(Xv_item),
    &xv_generic_pkg,
    item_init,
    item_set_avlist,
    item_get_attr,
    item_destroy,
    NULL			/* No find proc */
};
