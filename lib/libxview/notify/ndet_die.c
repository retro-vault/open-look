/*
 * Implements the ndet die routines used by the XView notify module.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#include <xview_private/ndet_die_.h>
#include <xview_private/ndet_death_.h>
#include <xview_private/ntfy_debug_.h>
#include <xview_private/ntfy_cond_.h>
#include <xview_private/ntfyprotec_.h>
#include <xview_private/ndet.h>
#include <xview_private/ndis.h>

extern int ntfy_errno_no_print;

extern          Notify_error
notify_die(status)
    Destroy_status  status;
{
    NTFY_ENUM       enum_code;
    Notify_error    return_code;

    if (ndet_check_status(status))
	return (NOTIFY_INVAL);
    NTFY_BEGIN_CRITICAL;

    /* 1067273 */
    ntfy_errno_no_print=1;
    /* Call all destroy procs (go around entire dispatch mechanism) */
    enum_code = ntfy_paranoid_enum_conditions(ndet_clients,
			   ndet_immediate_destroy, (NTFY_ENUM_DATA) status);
    ntfy_errno_no_print=0;
    /* If checking then return result */
    return_code = NOTIFY_OK;
    if (status == DESTROY_CHECKING) {
	if (enum_code == NTFY_ENUM_TERM)
	    return_code = NOTIFY_DESTROY_VETOED;
    } else if (status != DESTROY_SAVE_YOURSELF)
	/* else remove all clients */
	(void) ntfy_paranoid_enum_conditions(ndet_clients,
				      ndet_remove_all, NTFY_ENUM_DATA_NULL);
    NTFY_END_CRITICAL;
    return (return_code);
}

/*
 * Remove each client.
 */
/* ARGSUSED */
pkg_private     NTFY_ENUM
ndet_remove_all(client, condition, context)
    NTFY_CLIENT    *client;
    NTFY_CONDITION *condition;
    NTFY_ENUM_DATA  context;
{
    (void) notify_remove(client->nclient);
    return (NTFY_ENUM_SKIP);
}
