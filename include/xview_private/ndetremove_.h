/*
 * Declares private XView interfaces, types, and macros for ndetremove.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(NDETREMOVE__H)
#define NDETREMOVE__H

#include <xview/pkg.h>
#include <xview_private/ntfy.h>


Notify_error notify_remove(Notify_client nclient);

#endif
