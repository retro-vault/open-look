/*
 * Implements the ndetsingle routines used by the XView notify module.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#include <xview_private/ndetsingle_.h>
#include <xview_private/ndet.h>
#include <xview_private/ndis.h>	/* For ndis_client == NTFY_CLIENT_NULL check */
#include <errno.h>

extern          Notify_error
notify_dispatch()
{
    Notify_error    return_code;

    /*
     * Do nothing if in middle of notification loop or no other clients and
     * no notifications pending.
     */
    if (ndet_flags & NDET_STARTED)
	return (NOTIFY_INVAL);
    if ((ndet_clients == NTFY_CLIENT_NULL) &&
	(ndis_clients == NTFY_CLIENT_NULL))
	return (NOTIFY_NO_CONDITION);
    /* Setup flag to break out of notify_start after single loop */
    ndet_flags |= NDET_NO_DELAY;
    /* Start notifier */
    return_code = notify_start();
    /* Reset break out flag  */
    ndet_flags &= ~NDET_NO_DELAY;
    return (return_code);
}
