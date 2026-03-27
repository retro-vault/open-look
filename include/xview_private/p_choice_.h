/*
 * Declares private XView interfaces, types, and macros for p choice.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(P_CHOICE__H)
#define P_CHOICE__H

#include <xview/pkg.h>
#include <xview_private/panel_impl.h>

Pkg_private int choice_init(Panel panel_public, Panel_item item_public, Attr_avlist avlist);
Pkg_private Xv_opaque choice_set_avlist(Panel_item item_public, Attr_avlist avlist);
Pkg_private Xv_opaque choice_get_attr(Panel_item item_public, int *status, register Attr_attribute which_attr, va_list valist);
Pkg_private int choice_destroy(Panel_item item_public, Destroy_status status);

#endif
