/*
 * Declares public XView interfaces, types, and attributes for nint n
 * in.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(NINT_N_IN__H)
#define NINT_N_IN__H

#include <xview/pkg.h>
#include <xview_private/ntfy.h>

Notify_value notify_next_input_func(Notify_client nclient, int fd);

#endif
