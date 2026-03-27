/*
 * Declares public XView interfaces, types, and attributes for nint
 * set.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(NINT_SET__H)
#define NINT_SET__H

#include <xview/pkg.h>
#include <xview_private/ntfy.h>

pkg_private Notify_func nint_set_func(register NTFY_CONDITION *cond, Notify_func new_func);

#endif
