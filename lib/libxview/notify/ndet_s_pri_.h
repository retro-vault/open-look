/*
 * Declares internal interfaces used by the XView notify module.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(NDET_S_PRI__H)
#define NDET_S_PRI__H

#include <xview/pkg.h>
#include <xview_private/ntfy.h>

Notify_func notify_set_prioritizer_func(Notify_client nclient, register Notify_func func);

#endif
