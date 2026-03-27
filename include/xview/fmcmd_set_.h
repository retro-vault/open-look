/*
 * Declares public XView interfaces, types, and attributes for fmcmd
 * set.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(FMCMD_SET__H)
#define FMCMD_SET__H

#include <xview/pkg.h>
#include <xview/frame.h>

Pkg_private Xv_opaque frame_cmd_set_avlist(Frame frame_public, Attr_attribute avlist[]);

#endif
