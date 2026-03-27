/*
 * Declares internal interfaces used by the XView pw module.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(MEM__H)
#define MEM__H

#include <xview/pkg.h>
#include <pixrect/pixrect.h>
#include <xview_private/xv_.h>


Xv_private Pixrect *xv_mem_point(int w, int h, int depth, short *image);
Xv_private Pixrect *xv_mem_create(int w, int h, int depth);
Xv_private int xv_mem_destroy(Pixrect* pr);

#endif
