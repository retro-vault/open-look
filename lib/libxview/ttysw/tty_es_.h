/*
 * Declares internal interfaces used by the XView ttysw module.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(TTY_ES__H)
#define TTY_ES__H

#include <xview/pkg.h>
#include <xview_private/es.h>
#include <xview_private/tty_impl.h>

Pkg_private Xv_opaque ts_create(Ttysw *ttysw, Es_handle original, Es_handle scratch);

#endif
