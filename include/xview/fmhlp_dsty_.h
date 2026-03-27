/*
 * Declares public XView interfaces, types, and attributes for fmhlp
 * dsty.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(FMHLP_DSTY__H)
#define FMHLP_DSTY__H

#include <xview/pkg.h>
#include <xview/frame.h>

Pkg_private int frame_help_destroy(Frame frame_public, Destroy_status status);

#endif
