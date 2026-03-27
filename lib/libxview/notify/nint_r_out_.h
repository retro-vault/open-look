/*
 * Declares internal interfaces used by the XView notify module.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(NINT_R_OUT__H)
#define NINT_R_OUT__H

#include <xview/pkg.h>
#include <xview_private/ntfy.h>

Notify_error notify_remove_output_func(Notify_client nclient, Notify_func func, int fd);

#endif
