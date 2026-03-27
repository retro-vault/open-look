/*
 * Declares public XView interfaces, types, and attributes for
 * ndetsexcep.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(NDETSEXCEP__H)
#define NDETSEXCEP__H

#include <xview/pkg.h>
#include <xview_private/ntfy.h>

Notify_func notify_set_exception_func(Notify_client nclient, Notify_func func, int fd);

#endif
