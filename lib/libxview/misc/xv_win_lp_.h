/*
 * Declares internal interfaces used by the XView misc module.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(XV_WIN_LP__H)
#define XV_WIN_LP__H

#include <xview/pkg.h>
#include <xview/frame.h>


Xv_public Xv_opaque xv_window_loop(Frame frame);
Xv_public void xv_window_return(Xv_opaque ret);

#endif
