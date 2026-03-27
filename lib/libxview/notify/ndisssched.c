/*
 * Implements the ndisssched routines used by the XView notify module.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#include <xview_private/ndisdsched_.h>
#include <xview_private/ndisssched_.h>
#include <xview_private/ntfyprotec_.h>
#include <xview_private/ndis.h>

extern Notify_value(*ndis_scheduler) ();

extern          Notify_func
notify_set_scheduler_func(scheduler_func)
    Notify_func     scheduler_func;
{
    register Notify_func old_func;

    NTFY_BEGIN_CRITICAL;
    old_func = ndis_scheduler;
    ndis_scheduler = scheduler_func;
    if (ndis_scheduler == NOTIFY_FUNC_NULL)
	ndis_scheduler = ndis_default_scheduler;
    NTFY_END_CRITICAL;
    return (old_func);
}
