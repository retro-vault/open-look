/*
 * Declares public XView interfaces, types, and attributes for nint i
 * out.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(NINT_I_OUT__H)
#define NINT_I_OUT__H

#include <xview/pkg.h>
#include <xview_private/ntfy.h>

Notify_error notify_interpose_output_func(Notify_client nclient, Notify_func func, int fd);

#endif
