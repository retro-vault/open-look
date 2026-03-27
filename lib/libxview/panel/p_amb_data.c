/*
 * Implements the p amb data routines used by the XView panel module.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#include <xview_private/p_ambtn_.h>

Xv_pkg          xv_panel_ambtn_pkg = {
    "Abbreviated Menu Button Item",
    ATTR_PKG_PANEL,
    sizeof(Xv_panel_ambtn),
    &xv_panel_item_pkg,
    panel_ambtn_init,
    panel_ambtn_set_avlist,
    NULL,
    panel_ambtn_destroy,
    NULL			/* no find proc */
};
