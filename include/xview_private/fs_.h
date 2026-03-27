/*
 * Declares private XView interfaces, types, and macros for fs.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(FS__H)
#define FS__H

#include <xview/pkg.h>
#include <xview/fullscreen.h>

Pkg_private int fullscreen_init_internal(Xv_opaque owner, Xv_fullscreen *fullscreen_public, Attr_avlist avlist, int *offset_ptr);
Pkg_private int fullscreen_destroy_internal(Fullscreen fullscreen_public, Destroy_status status);
Xv_private char *fullscreen_translate_report_code(int code);
Pkg_private void fullscreen_update_globals(void);

#endif
