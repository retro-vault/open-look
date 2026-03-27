/*
 * Declares private XView interfaces, types, and macros for om get.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(OM_GET__H)
#define OM_GET__H

#include <xview/pkg.h>
#include <xview/openmenu.h>


Pkg_private Xv_opaque menu_gets(Menu menu_public, int *status, Attr_attribute attr, va_list args);
Pkg_private Xv_opaque menu_item_gets(Menu_item menu_item_public, int *status, Attr_attribute attr, va_list args);

#endif
