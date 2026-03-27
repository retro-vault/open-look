/*
 * Declares private XView interfaces, types, and macros for fs get.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(FS_GET__H)
#define FS_GET__H

#include <xview/pkg.h>
#include <xview_private/fs_impl.h>


Pkg_private Xv_opaque fullscreen_get_attr(Fullscreen fullscreen_public, int *status, Fullscreen_attr attr, va_list args);

#endif
