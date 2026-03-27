/*
 * Declares private XView interfaces, types, and macros for fs compat.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(FS_COMAPAT__H)
#define FS_COMAPAT__H

#include <xview/pkg.h>
#include <xview/fullscreen.h>


Xv_public struct fullscreen *fullscreen_init(Xv_Window window);
Xv_public int fullscreen_set_cursor(struct fullscreen *fs, Xv_Cursor cursor);
Xv_public int fullscreen_set_inputmask(struct fullscreen *fs, Inputmask *im);
Xv_public int fullscreen_destroy(struct fullscreen *fs);

#endif
