/*
 * Implements the p lst data routines used by the XView panel module.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#include <xview_private/p_list_.h>

Xv_pkg          xv_panel_list_pkg = {
    "Panel_list Item", ATTR_PKG_PANEL,
    sizeof(Xv_panel_list),
    &xv_panel_item_pkg,
    panel_list_init,
    panel_list_set_avlist,
    panel_list_get_attr,
    panel_list_destroy,
    NULL			/* no find proc */
};
