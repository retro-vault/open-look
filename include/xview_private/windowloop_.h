/*
 * Declares private XView interfaces, types, and macros for windowloop.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(WINDOWLOOP__H)
#define WINDOWLOOP__H

#include <xview/pkg.h>
#include <xview/frame.h>

Xv_private Xv_opaque _xv_block_loop(register Frame frame);
Xv_private void _xv_block_return(Xv_opaque value);

#endif
