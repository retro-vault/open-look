/*
 * Declares private XView interfaces, types, and macros for nint r sig.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(NINT_R_SIG__H)
#define NINT_R_SIG__H

#include <xview/pkg.h>
#include <xview_private/ntfy.h>

Notify_error notify_remove_signal_func(Notify_client nclient, Notify_func func, int signal, Notify_signal_mode mode);

#endif
