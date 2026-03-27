/*
 * Declares private XView interfaces, types, and macros for ndetgdeath.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(NDETGDEATH__H)
#define NDETGDEATH__H

#include <xview/pkg.h>
#include <xview_private/ntfy.h>

Notify_func notify_get_destroy_func(Notify_client nclient);

#endif
