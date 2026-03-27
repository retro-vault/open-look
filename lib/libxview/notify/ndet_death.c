/*
 * Implements the ndet death routines used by the XView notify module.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#include <xview_private/ndet_death_.h>
#include <xview_private/nint_stack_.h>
#include <xview_private/ntfy_debug_.h>
#include <xview_private/ntfyprotec_.h>
#include <xview_private/ndet.h>
#include <xview_private/ndis.h>
#include <xview_private/nint.h>

pkg_private int
ndet_check_status(status)
    Destroy_status  status;
{
    if (!(status == DESTROY_PROCESS_DEATH || status == DESTROY_CHECKING ||
	  status == DESTROY_CLEANUP || status == DESTROY_SAVE_YOURSELF)) {
	ntfy_set_errno(NOTIFY_INVAL);
	return (-1);
    }
    return (0);
}

/*
 * Immediate destroy notification.
 */
pkg_private     NTFY_ENUM
ndet_immediate_destroy(client, condition, context)
    NTFY_CLIENT    *client;
    register NTFY_CONDITION *condition;
    NTFY_ENUM_DATA  context;
{
    Destroy_status  status = (Destroy_status) context;

    if (condition->type == NTFY_DESTROY) {
	Notify_func     func = nint_push_callout(client, condition);

	/* Send destroy notification */
	ndet_flags &= ~NDET_VETOED;
	NTFY_END_CRITICAL;
	(void) func(client->nclient, status);
	NTFY_BEGIN_CRITICAL;
	nint_unprotected_pop_callout();
	/* Return NTFY_ENUM_TERM if checking and was told to veto */
	if (status == DESTROY_CHECKING && (ndet_flags & NDET_VETOED))
	    return (NTFY_ENUM_TERM);
	else
	    /*
	     * Since only one destroy per client can skip other conditions.
	     */
	    return (NTFY_ENUM_SKIP);
    }
    return (NTFY_ENUM_NEXT);
}
