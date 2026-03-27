/*
 * Declares private XView interfaces, types, and macros for ndet value.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(NDET_VALUE__H)
#define NDET_VALUE__H

#include <xview/pkg.h>
#include <xview_private/ntfy.h>

Notify_error notify_itimer_value(Notify_client nclient, int which, struct itimerval *value);

#endif
