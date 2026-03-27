/*
 * Declares internal interfaces used by the XView canvas module.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(CNVS_RESZE__H)
#define CNVS_RESZE__H

#include <xview/pkg.h>
#include <xview/base.h>
#include <xview_private/cnvs_impl.h>

Pkg_private void canvas_resize_paint_window(Canvas_info *canvas, int width, int height);

#endif
