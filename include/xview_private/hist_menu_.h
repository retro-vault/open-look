/*
 * Declares private XView interfaces, types, and macros for hist menu.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(HIST_MENU__H)
#define HIST_MENU__H

#include <xview/pkg.h>
#include <xview/attrol.h>
#include <xview/generic.h>
#include <xview/hist.h>

Pkg_private int hist_menu_init(Xv_opaque owner, History_menu_public *public, Attr_avlist avlist);
Pkg_private Xv_opaque hist_menu_set(History_menu public, Attr_avlist avlist);
Pkg_private Xv_opaque hist_menu_get(History_menu_public *public, int *status, Attr_attribute attr, Attr_avlist args);
Pkg_private int hist_menu_destroy(History_menu public, Destroy_status statusaaa);

#endif
