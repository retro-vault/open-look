/*
 * Declares public XView interfaces, types, and attributes for fmhlp
 * get.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(FMHLP_GET__H)
#define FMHLP_GET__H

#include <xview/pkg.h>
#include <xview/frame.h>

Pkg_private Xv_opaque frame_help_get_attr(Frame frame_public, int *status, Frame_attribute attr, va_list valist);

#endif
