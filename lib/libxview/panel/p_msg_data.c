/*
 * Implements the p msg data routines used by the XView panel module.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#include <xview_private/p_msg_.h>

Xv_pkg          xv_panel_message_pkg = {
    "Message Item", ATTR_PKG_PANEL,
    sizeof(Xv_panel_message),
    &xv_panel_item_pkg,
    panel_message_init,
    NULL,
    NULL,
    NULL,
    NULL			/* no find proc */
};
