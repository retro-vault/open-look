/*
 * Declares public XView interfaces, types, and attributes for frame
 * init.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(FRAME_INIT__H)
#define FRAME_INIT__H

#include <xview/pkg.h>
#include <xview/frame.h>
#include <xview_private/fm_impl.h>


Pkg_private void frame_default_done_func(Frame frame);
Pkg_private int frame_gravity_from_flags(int flags);
Pkg_private int frame_init(Xv_Window owner, Frame frame_public, Attr_avlist	 avlist);
Pkg_private Xv_window frame_create_footer(Frame_class_info *frame);
#ifdef OW_I18N
Pkg_private Xv_window frame_create_IMstatus(Frame_class_info *frame);
#endif

#endif
