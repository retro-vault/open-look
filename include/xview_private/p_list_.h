/*
 * Declares private XView interfaces, types, and macros for p list.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(P_LIST__H)
#define P_LIST__H

#include <xview/pkg.h>
#include <xview_private/panel_impl.h>

Pkg_private int panel_list_init(Xv_Window parent, Panel_item panel_list_public, Attr_avlist avlist);
Pkg_private Xv_opaque panel_list_set_avlist(Panel_item panel_list_public, Attr_avlist avlist);
Pkg_private Xv_opaque panel_list_get_attr(Panel_item panel_list_public, int *status, Attr_attribute which_attr, va_list valist);
Pkg_private int panel_list_destroy(Panel_item item_public, Destroy_status status);

#endif
