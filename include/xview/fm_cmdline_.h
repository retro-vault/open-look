/*
 * Declares public XView interfaces, types, and attributes for fm
 * cmdline.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(FM_CMDLINE__H)
#define FM_CMDLINE__H

#include <xview/pkg.h>
#include <xview/frame.h>

Pkg_private int frame_set_cmdline_options(Frame frame_public, Bool from_init);
Pkg_private int frame_all_set_cmdline_options(Frame frame_public);
Pkg_private int frame_set_icon_cmdline_options(Frame frame_public);

static int frame_parse_color(Frame frame, char *colorname, XColor *xcolor);

#endif
