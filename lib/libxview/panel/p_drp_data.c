/*
 * Implements the p drp data routines used by the XView panel module.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#include <xview_private/p_drop_.h>

Xv_pkg          xv_panel_drop_pkg = {
    "Drop Target Item",
    ATTR_PKG_PANEL,
    sizeof(Xv_panel_drop),
    &xv_panel_item_pkg,
    panel_drop_init,
    panel_drop_set_avlist,
    panel_drop_get_attr,
    panel_drop_destroy,
    NULL			/* no find proc */
};
