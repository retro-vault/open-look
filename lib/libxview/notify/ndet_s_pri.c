/*
 * Implements the ndet s pri routines used by the XView notify module.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#include <xview_private/ndet_s_pri_.h>
#include <xview_private/ndis_d_pri_.h>
#include <xview_private/ntfyclient_.h>
#include <xview_private/ntfyprotec_.h>
#include <xview_private/ndet.h>
#include <xview_private/ndis.h>	/* For ndis_default_prioritizer */

extern          Notify_func
notify_set_prioritizer_func(nclient, func)
    Notify_client   nclient;
    register Notify_func func;
{
    register Notify_func old_func = NOTIFY_FUNC_NULL;
    register NTFY_CLIENT *client;

    NTFY_BEGIN_CRITICAL;
    /* Find/create client that corresponds to nclient */
    if ((client = ntfy_new_nclient(&ndet_clients, nclient,
				   &ndet_client_latest)) == NTFY_CLIENT_NULL)
	goto Done;
    /* Exchange functions */
    old_func = client->prioritizer;
    client->prioritizer = func;
    /* Use default if null */
    if (func == NOTIFY_FUNC_NULL)
	client->prioritizer = ndis_default_prioritizer;
Done:
    NTFY_END_CRITICAL;
    return (old_func);
}
