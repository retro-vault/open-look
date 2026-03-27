/*
 * Implements the nintndeath routines used by the XView notify module.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#include <xview_private/nintndeath_.h>
#include <xview_private/ndet_death_.h>
#include <xview_private/nint_next_.h>
#include <xview_private/ndet.h>
#include <xview_private/nint.h>

extern          Notify_value
notify_next_destroy_func(nclient, status)
    Notify_client   nclient;
    Destroy_status  status;
{
    Notify_func     func;

    /* Check arguments */
    if (ndet_check_status(status))
	return (NOTIFY_UNEXPECTED);
    if ((func = nint_next_callout(nclient, NTFY_DESTROY)) ==
	NOTIFY_FUNC_NULL)
	return (NOTIFY_UNEXPECTED);
    return (func(nclient, status));
}
