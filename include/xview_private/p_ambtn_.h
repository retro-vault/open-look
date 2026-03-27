/*
 * Declares private XView interfaces, types, and macros for p ambtn.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(P_AMBTN__H)
#define P_AMBTN__H

#include <xview/pkg.h>
#include <xview_private/panel_impl.h>

Pkg_private int panel_ambtn_init(Panel panel_public, Panel_item item_public, Attr_avlist avlist);
Pkg_private Xv_opaque panel_ambtn_set_avlist(Panel_item item_public, Attr_avlist avlist);
Pkg_private int panel_ambtn_destroy(Panel_item item_public, Destroy_status status);

#endif
