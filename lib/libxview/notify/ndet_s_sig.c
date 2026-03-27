/*
 * Implements the ndet s sig routines used by the XView notify module.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#include <xview_private/ndet_s_sig_.h>
#include <xview_private/ndet_sig_.h>
#include <xview_private/ndet_loop_.h>
#include <xview_private/ndisdispch_.h>
#include <xview_private/nint_set_.h>
#include <xview_private/nint_stack_.h>
#include <xview_private/ntfyclient_.h>
#include <xview_private/ntfy_cond_.h>
#include <xview_private/ntfy_ctbl_.h>
#include <xview_private/ntfyprotec_.h>
#include <xview_private/ndet.h>
#include <xview_private/ndis.h>
#include <xview_private/nint.h>

extern          Notify_func
notify_set_signal_func(nclient, func, sig, mode)
    Notify_client   nclient;
    Notify_func     func;
    int             sig;
    Notify_signal_mode mode;
{
    Notify_func     old_func = NOTIFY_FUNC_NULL;
    register NTFY_CLIENT *client;
    NTFY_CONDITION *condition;
    NTFY_TYPE       type;

    NTFY_BEGIN_CRITICAL;
    /*
     * Check args & pre-allocate stack incase going to get asynchronous event
     * before synchronous one.
     */
    if (ndet_check_mode(mode, &type) || ndet_check_sig(sig) ||
	(nint_alloc_stack() != NOTIFY_OK))
	goto Done;
    /* Find/create client that corresponds to nclient */
    if ((client = ntfy_new_nclient(&ndet_clients, nclient,
				   &ndet_client_latest)) == NTFY_CLIENT_NULL)
	goto Done;
    /* Find/create condition */
    if ((condition = ntfy_new_condition(&(client->conditions), type,
	    &(client->condition_latest), (NTFY_DATA)(long)sig, NTFY_USE_DATA)) ==
	NTFY_CONDITION_NULL)
	goto Done;
    ntfy_add_to_table(client, condition, type);
    /* Exchange functions */
    old_func = nint_set_func(condition, func);
    /* Remove condition if func is null */
    if (func == NOTIFY_FUNC_NULL) {
	ndis_flush_condition(nclient, type,
			     (NTFY_DATA)(long)sig, NTFY_USE_DATA);
	ntfy_unset_condition(&ndet_clients, client, condition,
			     &ndet_client_latest, NTFY_NDET);
    } else
	/* Make sure we are catching sig before return */
	ndet_enable_sig(sig);
    /*
     * Have notifier check for signal changes next time around loop. Will
     * confirm signal handling at this time.
     */
    ndet_flags |= NDET_SIGNAL_CHANGE;
Done:
    NTFY_END_CRITICAL;
    return (old_func);
}
