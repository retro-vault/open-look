/*
 * Declares internal interfaces used by the XView panel module.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(P_DROP__H)
#define P_DROP__H

#include <xview/pkg.h>
#include <xview_private/panel_impl.h>

Pkg_private int panel_drop_init(Panel panel_public, Panel_item item_public, Attr_avlist avlist);
Pkg_private Xv_opaque panel_drop_set_avlist(Panel_item item_public, Attr_avlist avlist);
Pkg_private Xv_opaque panel_drop_get_attr(Panel_item item_public, int *status, register Attr_attribute which_attr, va_list avlist);
Pkg_private int panel_drop_destroy(Panel_item item, Destroy_status status);

#endif
