/*
 * Declares internal interfaces used by the XView notify module.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(NDISGSCHED__H)
#define NDISGSCHED__H

#include <xview/pkg.h>
#include <xview_private/ntfy.h>

Notify_func notify_get_scheduler_func(void);

#endif
