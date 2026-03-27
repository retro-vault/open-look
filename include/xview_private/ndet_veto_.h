/*
 * Declares private XView interfaces, types, and macros for ndet veto.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(NDET_VETO__H)
#define NDET_VETO__H

#include <xview/pkg.h>
#include <xview_private/ntfy.h>

Notify_error notify_veto_destroy(Notify_client nclient);

#endif
