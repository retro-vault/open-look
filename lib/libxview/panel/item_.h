/*
 * Declares internal interfaces used by the XView panel module.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(ITEM__H)
#define ITEM__H

#include <xview/pkg.h>
#include <xview_private/panel_impl.h>


Pkg_private int item_init( Xv_Window parent, Panel_item item_public, Attr_avlist avlist);
Pkg_private int item_destroy(Panel_item item_item_public, Destroy_status status);

#endif
