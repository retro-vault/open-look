/*
 * Declares public XView interfaces, types, and attributes for frame
 * help.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(FRAME_HELP__H)
#define FRAME_HELP__H

#include <xview/pkg.h>
#include <xview/frame.h>


Pkg_private int frame_help_init(Xv_Window owner, Frame frame_public, Frame_attribute avlist[]);

#endif
