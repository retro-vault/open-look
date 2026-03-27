/*
 * Declares internal interfaces used by the XView scrollbar module.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(SB_DESTROY__H)
#define SB_DESTROY__H

#include <xview/pkg.h>
#include <xview_private/sb_impl.h>

Pkg_private int scrollbar_destroy_internal(Scrollbar sb_public, Destroy_status status);

#endif
