/*
 * Declares internal interfaces used by the XView fullscreen module.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(FS_SET__H)
#define FS_SET__H

#include <xview/pkg.h>
#include <xview/fullscreen.h>


Pkg_private Xv_opaque fullscreen_set_avlist(Xv_fullscreen  *fullscreen_public, Attr_avlist avlist);

#endif
