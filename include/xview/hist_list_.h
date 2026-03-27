/*
 * Declares public XView interfaces, types, and attributes for hist
 * list.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(HIST_LIST__H)
#define HIST_LIST__H

#include <xview/pkg.h>
#include <xview/attrol.h>
#include <xview/generic.h>
#include <xview/hist.h>

Pkg_private int hist_list_init(Xv_opaque owner, History_list_public *public, Attr_avlist avlist);
Pkg_private Xv_opaque hist_list_set(History_list public, Attr_avlist avlist);
Pkg_private Xv_opaque hist_list_get(History_list_public *public, int *status, Attr_attribute attr, va_list args);
Pkg_private Xv_object hist_list_find(Xv_opaque parent, Xv_pkg *pkg, Attr_avlist avlist);
Pkg_private int hist_list_destroy(History_list_public *public, Destroy_status status);

#endif
