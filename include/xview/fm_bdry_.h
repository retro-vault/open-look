/*
 * Declares public XView interfaces, types, and attributes for fm bdry.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(FM_BDRY__H)
#define FM_BDRY__H

#include <xview/pkg.h>
#include <xview_private/fm_impl.h>

Pkg_private void frame_compute_constraint(Frame_class_info *frame, Xv_Window target_sw, register Rect *rconstrain);

#endif
