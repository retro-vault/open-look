/*
 * Implements the ndet g pri routines used by the XView notify module.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#include <xview_private/ndet_g_pri_.h>
#include <xview_private/ntfyclient_.h>
#include <xview_private/ntfy_debug_.h>
#include <xview_private/ntfyprotec_.h>
#include <xview_private/ndet.h>

extern          Notify_func
notify_get_prioritizer_func(nclient)
    Notify_client   nclient;
{
    register NTFY_CLIENT *client;
    register Notify_func func = NOTIFY_FUNC_NULL;

    NTFY_BEGIN_CRITICAL;
    /* Find client that corresponds to nclient */
    if ((client = ntfy_find_nclient(ndet_clients, nclient,
				&ndet_client_latest)) == NTFY_CLIENT_NULL) {
	ntfy_set_errno(NOTIFY_UNKNOWN_CLIENT);
	goto Done;
    }
    /* Get function */
    func = client->prioritizer;
Done:
    NTFY_END_CRITICAL;
    return (func);
}
