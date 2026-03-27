/*
 * Declares private XView interfaces, types, and macros for path.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(PATH__H)
#define PATH__H

#include <xview/pkg.h>
#include <xview/attrol.h>
#include <xview/generic.h>
#include <xview/panel.h>
#include <xview/path.h>

Pkg_private int path_init_avlist(Xv_opaque owner, Path_public *public, Attr_avlist avlist);
Pkg_private Xv_opaque path_set_avlist(Path_name public, Attr_avlist avlist);
Pkg_private Xv_opaque path_get_attr(Path_public *public, int *status, Attr_attribute attr, Attr_avlist args);
Pkg_private int path_destroy_private(Path_public *public, Destroy_status status);
Pkg_private Panel_setting xv_path_name_notify_proc(Path_name item, Event *event);

#endif
