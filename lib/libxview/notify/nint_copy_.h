/*
 * Declares internal interfaces used by the XView notify module.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(NINT_COPY__H)
#define NINT_COPY__H

#include <xview/pkg.h>
#include <xview_private/ntfy.h>

pkg_private Notify_error nint_copy_callout(NTFY_CONDITION *new_cond, NTFY_CONDITION *old_cond);

#endif
