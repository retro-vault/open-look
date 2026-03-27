/*
 * Declares private XView interfaces, types, and macros for ndisssched.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(NDISSSCHED__H)
#define NDISSSCHED__H

#include <xview/pkg.h>
#include <xview_private/ntfy.h>

Notify_func notify_set_scheduler_func(Notify_func scheduler_func);

#endif
