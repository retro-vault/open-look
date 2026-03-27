/*
 * Declares public XView interfaces, types, and attributes for panel.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(PANEL__H)
#define PANEL__H

#include <xview/pkg.h>
#include <xview_private/panel_impl.h>

Pkg_private int panel_init(Xv_Window parent, Xv_Window panel_public, Attr_avlist avlist);
Pkg_private int panel_destroy(Panel panel_public, Destroy_status status);
Pkg_private void panel_register_view(Panel_info *panel, Xv_Window view);

#endif
