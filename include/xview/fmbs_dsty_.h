/*
 * Declares public XView interfaces, types, and attributes for fmbs
 * dsty.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(FMBS_DSTY__H)
#define FMBS_DSTY__H

#include <xview/pkg.h>
#include <xview_private/frame_base.h>

Pkg_private int frame_base_destroy(Frame frame_public, Destroy_status status);

#endif
