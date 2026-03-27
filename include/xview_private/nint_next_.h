/*
 * Declares private XView interfaces, types, and macros for nint next.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(NINT_NEXT__H)
#define NINT_NEXT__H

#include <xview/pkg.h>
#include <xview_private/ntfy.h>

pkg_private Notify_func nint_next_callout(Notify_client nclient, NTFY_TYPE type);

#endif
