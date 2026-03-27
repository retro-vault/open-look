/*
 * Declares public XView interfaces, types, and attributes for
 * nintnevent.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(NINTNEVENT__H)
#define NINTNEVENT__H

#include <xview/pkg.h>
#include <xview_private/ntfy.h>

Notify_value notify_next_event_func(Notify_client nclient, Notify_event event, Notify_arg arg, Notify_event_type when);

#endif
