/*
 * Declares public XView interfaces, types, and attributes for
 * ndetgevent.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(NDETGEVENT__H)
#define NDETGEVENT__H

#include <xview/pkg.h>
#include <xview_private/ntfy.h>

Notify_func notify_get_event_func(Notify_client nclient, Notify_event_type when);

#endif
