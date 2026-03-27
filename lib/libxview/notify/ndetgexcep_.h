/*
 * Declares internal interfaces used by the XView notify module.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(NDETGEXCEP__H)
#define NDETGEXCEP__H

#include <xview/pkg.h>
#include <xview_private/ntfy.h>

Notify_func notify_get_exception_func(Notify_client nclient, int fd);

#endif
