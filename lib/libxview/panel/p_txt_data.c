/*
 * Implements the p txt data routines used by the XView panel module.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#include <xview_private/p_txt_.h>

Xv_pkg          xv_panel_text_pkg = {
    "Text Item", ATTR_PKG_PANEL,
    sizeof(Xv_panel_text),
    &xv_panel_item_pkg,
    text_init,
    text_set_avlist,
    text_get_attr,
    text_destroy,
    NULL			/* no find proc */
};
