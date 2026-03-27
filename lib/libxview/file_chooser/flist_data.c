/*
 * Implements the flist data routines used by the XView file_chooser
 * module.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#include <xview/xview.h>
#include <xview_private/file_list_.h>

Xv_pkg file_list_pkg = {
    "File_list",
    ATTR_PKG_FILE_LIST,
    sizeof(File_list_public),
    PANEL_LIST,
    file_list_init,
    file_list_set,
    file_list_get,
    file_list_destroy,
    NULL                     /* no find */
};
