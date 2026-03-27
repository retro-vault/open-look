/*
 * Implements the p btn data routines used by the XView panel module.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#include <xview_private/p_btn_.h>

Xv_pkg          xv_panel_button_pkg = {
    "Button Item", ATTR_PKG_PANEL,
    sizeof(Xv_panel_button),
    &xv_panel_item_pkg,
    panel_button_init,
    NULL,
    NULL,
    panel_button_destroy,
    NULL			/* no find proc */
};
