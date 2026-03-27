/*
 * Declares internal interfaces used by the XView frame module.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(FM_WIN__H)
#define FM_WIN__H

#include <xview/pkg.h>
#include <xview/frame.h>
#include <xview_private/fm_impl.h>


Pkg_private void frame_set_position(unsigned long parent, Frame_class_info *frame);
Pkg_private int frame_is_exposed(Frame frame);

#endif
