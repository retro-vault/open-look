/*
 * Declares public XView interfaces, types, and attributes for
 * ndetpdeath.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(NDETPDEATH__H)
#define NDETPDEATH__H

#include <xview/pkg.h>
#include <xview_private/ntfy.h>

Notify_error notify_post_destroy(Notify_client nclient, Destroy_status status, Notify_event_type when);

#endif
