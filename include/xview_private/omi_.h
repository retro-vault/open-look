/*
 * Declares private XView interfaces, types, and macros for omi.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(OMI__H)
#define OMI__H

#include <xview/pkg.h>
#include <xview_private/om_impl.h>

Pkg_private int menu_image_compute_size(Xv_menu_info *m, register struct image *im, register struct image *std_image);

#endif
