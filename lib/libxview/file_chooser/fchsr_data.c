/*
 * Implements the fchsr data routines used by the XView file_chooser
 * module.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#include <xview/xview.h>
#include <xview_private/file_chsr_.h>

Xv_pkg file_chooser_pkg = {
    "File_chooser",
    ATTR_PKG_FILE_CHOOSER,
    sizeof(File_chooser_public),
    FRAME_CMD,
    file_chooser_init,
    file_chooser_set,
    file_chooser_get,
    file_chooser_destroy,
    NULL                     /* no find */
};
