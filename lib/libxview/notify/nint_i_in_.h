/*
 * Declares internal interfaces used by the XView notify module.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(NINT_I_IN__H)
#define NINT_I_IN__H

#include <xview/pkg.h>
#include <xview_private/ntfy.h>

Notify_error notify_interpose_input_func(Notify_client nclient, Notify_func func, int fd);

#endif
