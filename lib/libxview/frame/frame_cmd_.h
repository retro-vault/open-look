/*
 * Declares internal interfaces used by the XView frame module.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(FRAME_CMD__H)
#define FRAME_CMD__H

#include <xview/pkg.h>
#include <xview/frame.h>

Pkg_private int frame_cmd_init(Xv_Window owner, Frame frame_public, Attr_attribute avlist[]);
Pkg_private Notify_value frame_cmd_input(Frame frame_public, Event *event, Notify_arg arg, Notify_event_type type);

#endif
