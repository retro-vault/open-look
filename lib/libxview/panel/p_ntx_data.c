/*
 * Implements the p ntx data routines used by the XView panel module.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#include <xview_private/p_num_txt_.h>

Xv_pkg xv_panel_num_text_pkg = {
    "Numeric Text Item", ATTR_PKG_PANEL,
    sizeof(Xv_panel_num_text),
    &xv_panel_item_pkg,
    panel_num_text_init,
    panel_num_text_set_avlist,
    panel_num_text_get_attr,
    panel_num_text_destroy,
    NULL                        /* no find proc */
};
