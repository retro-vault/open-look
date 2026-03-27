/*
 * Declares private XView interfaces, types, and macros for nint get.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(NINT_GET__H)
#define NINT_GET__H

#include <xview/pkg.h>
#include <xview_private/ntfy.h>

pkg_private Notify_func nint_get_func(register NTFY_CONDITION *cond);

#endif
