/*
 * Declares private XView interfaces, types, and macros for nint stack.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(NINT_STACK__H)
#define NINT_STACK__H

#include <xview/pkg.h>
#include <xview_private/ntfy.h>

pkg_private Notify_func nint_push_callout(NTFY_CLIENT *client, NTFY_CONDITION *cond);
pkg_private void nint_pop_callout(void);
pkg_private void nint_unprotected_pop_callout(void);
pkg_private Notify_error nint_alloc_stack(void);

#endif
