/*
 * Declares public XView interfaces, types, and attributes for p view.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(P_VIEW__H)
#define P_VIEW__H

#include <xview/pkg.h>
#include <xview_private/panel_impl.h>

Pkg_private int panel_view_init(Panel parent, Panel_view view_public, Attr_attribute avlist[]);

#endif
