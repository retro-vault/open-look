/*
 * Declares public XView interfaces, types, and attributes for fm
 * rescale.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(FM_RESCALE__H)
#define FM_RESCALE__H

#include <xview/pkg.h>
#include <xview/frame.h>


Pkg_private void frame_rescale_subwindows(Frame frame_public, int scale);

#endif
