/*
 * Declares internal interfaces used by the XView notify module.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(NDETS_WAIT__H)
#define NDETS_WAIT__H

#include <xview/pkg.h>
#include <xview_private/ntfy.h>

Notify_func notify_set_wait3_func(Notify_client nclient, Notify_func func, int pid);

#endif
