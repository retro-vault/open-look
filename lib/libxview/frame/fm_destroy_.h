/*
 * Declares internal interfaces used by the XView frame module.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(FM_DESTROY__H)
#define FM_DESTROY__H

#include <xview/pkg.h>
#include <xview/frame.h>

Pkg_private int frame_destroy(Frame frame_public, Destroy_status status);

#endif
