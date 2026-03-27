/*
 * Declares private XView interfaces, types, and macros for cnvs view.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(CNVS_VIEW__H)
#define CNVS_VIEW__H

#include <xview/pkg.h>
#include <xview_private/cnvs_impl.h>

Pkg_private int canvas_view_init(Canvas parent, Canvas_view view_public, Attr_attribute avlist[]);
Pkg_private Xv_opaque canvas_view_get(Canvas_view view_public, int *stat, Attr_attribute attr, va_list valist);
Pkg_private Xv_opaque canvas_paint_get(Canvas_paint_window paint_public, int *stat, Attr_attribute attr, va_list valist);
Pkg_private Xv_opaque canvas_paint_set(Canvas_paint_window paint_public, Attr_avlist avlist);
Pkg_private int canvas_view_destroy(Canvas_view view_public, Destroy_status stat);

#endif
