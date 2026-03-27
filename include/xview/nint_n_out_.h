/*
 * Declares public XView interfaces, types, and attributes for nint n
 * out.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(NINT_N_OUT__H)
#define NINT_N_OUT__H

#include <xview/pkg.h>
#include <xview_private/ntfy.h>

Notify_value notify_next_output_func(Notify_client nclient, int fd);

#endif
