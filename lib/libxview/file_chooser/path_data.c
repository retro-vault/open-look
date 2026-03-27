/*
 * Implements the path data routines used by the XView file_chooser
 * module.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#include <xview/xview.h>
#include <xview_private/path_.h>

Xv_pkg path_pkg = {
    "Path_name",
    ATTR_PKG_PATH,
    sizeof(Path_public),
    PANEL_TEXT,
    path_init_avlist,
    path_set_avlist,
    path_get_attr,
    path_destroy_private,
    NULL                     /* no find */
};
