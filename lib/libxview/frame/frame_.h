/*
 * Declares internal interfaces used by the XView frame module.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(FRAME__H)
#define FRAME__H

#include <xview/pkg.h>
#include <xview/frame.h>

Xv_private void frame_cmdline_help(char *name);
Xv_public void frame_get_rect(Frame frame, Rect *rect);
Xv_public void frame_set_rect(Frame frame, Rect *rect);

#endif
