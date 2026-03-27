/*
 * Declares public XView interfaces, types, and attributes for p
 * compat.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(P_COMPAT__H)
#define P_COMPAT__H

#include <xview/pkg.h>
#include <xview_private/panel_impl.h>

Sv1_public Panel_item _panel_create_item(Panel client_panel, Xv_pkg *item_type, ...);
Sv1_public Panel_attribute_value _panel_get(Panel client_object, Panel_attr attr, ...);
Sv1_public int _panel_set(Panel client_object, ...);
Sv1_public void panel_destroy_item(Panel_item client_item);
Sv1_public void panel_free(Panel client_object);

#endif
