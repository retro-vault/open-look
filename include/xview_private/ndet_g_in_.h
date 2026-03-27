/*
 * Declares private XView interfaces, types, and macros for ndet g in.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(NDET_G_IN__H)
#define NDET_G_IN__H

#include <xview/pkg.h>
#include <xview_private/ntfy.h>

Notify_func notify_get_input_func(Notify_client nclient, int fd);

#endif
