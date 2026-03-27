/*
 * Declares private XView interfaces, types, and macros for item get.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(ITEM_GET__H)
#define ITEM_GET__H

#include <xview/pkg.h>
#include <xview_private/panel_impl.h>

Pkg_private Xv_opaque item_get_attr(Panel_item item_public, int *status, register Attr_attribute which_attr, va_list valist);

#endif
