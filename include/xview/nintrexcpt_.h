/*
 * Declares public XView interfaces, types, and attributes for
 * nintrexcpt.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(NINTREXCPT__H)
#define NINTREXCPT__H

#include <xview/pkg.h>
#include <xview_private/ntfy.h>

Notify_error notify_remove_exception_func(Notify_client nclient, Notify_func func, int fd);

#endif
