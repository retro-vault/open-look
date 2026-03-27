/*
 * Implements the ndisgsched routines used by the XView notify module.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#include <xview_private/ndisgsched_.h>
#include <xview_private/ndis.h>

extern Notify_value(*ndis_scheduler) ();

extern          Notify_func
notify_get_scheduler_func()
{
    return (ndis_scheduler);
}
