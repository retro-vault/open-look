/*
 * Declares private XView interfaces, types, and macros for drawable.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(DRAWABLE__H)
#define DRAWABLE__H

#include <xview/pkg.h>
#include <xview/drawable.h>
#include <X11/Xresource.h>

Pkg_private int drawable_init(Xv_opaque parent_public, Xv_drawable_struct *drawable_public, Attr_avlist avlist, int *offset_ptr);
Pkg_private int drawable_destroy(register Xv_Drawable drawable_public, Destroy_status status);
Pkg_private Xv_opaque drawable_get_attr(Xv_Drawable drawable_public, int *status, Drawable_attr attr);
Xv_private GC xv_private_gc(Xv_opaque d);

#endif
