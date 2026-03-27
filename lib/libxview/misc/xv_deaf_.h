/*
 * Declares internal interfaces used by the XView misc module.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(XV_DEAF__H)
#define XV_DEAF__H

#include <xview/pkg.h>
#include <xview/window.h>


int xv_deaf(Xv_window parent, Bool on);

#endif
