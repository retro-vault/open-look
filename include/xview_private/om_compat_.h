/*
 * Declares private XView interfaces, types, and macros for om compat.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(OM_COMPAT__H)
#define OM_COMPAT__H

#include <xview/pkg.h>
#include <xview/openmenu.h>


Sv1_public Menu _menu_create(Attr_attribute attr1, ...);
Sv1_public Menu_item _menu_create_item(Attr_attribute attr1, ...);
Sv1_public Xv_opaque _menu_set(Menu menu_public, ...);
Sv1_public Xv_opaque menu_get(Menu menu_menu_public, Xv_opaque attr, Xv_opaque v1);
Sv1_public void menu_destroy_with_proc(Menu m_public, void (*destroy_proc)());

#endif
