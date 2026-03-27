/*
 * Declares private XView interfaces, types, and macros for ndetsevent.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(NDETSEVENT__H)
#define NDETSEVENT__H

#include <xview/pkg.h>
#include <xview_private/ntfy.h>

Notify_func notify_set_event_func(Notify_client nclient, Notify_func func, Notify_event_type when);

#endif
