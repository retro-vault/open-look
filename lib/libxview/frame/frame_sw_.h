/*
 * Declares internal interfaces used by the XView frame module.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(FRAME_SW__H)
#define FRAME_SW__H

#include <xview/pkg.h>
#include <xview/frame.h>

Pkg_private void frame_layout_subwindows(Frame frame_public);
#ifdef OW_I18N
#ifdef FULL_R5
Pkg_private void frame_update_status_subwindows(Frame frame_public, Xv_Window sw);
#endif 
#endif 

#endif
