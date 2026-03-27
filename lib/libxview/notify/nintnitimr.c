/*
 * Implements the nintnitimr routines used by the XView notify module.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#include <xview_private/nintnitimr_.h>
#include <xview_private/ndetitimer_.h>
#include <xview_private/nint_next_.h>
#include <xview_private/ndet.h>
#include <xview_private/nint.h>

extern          Notify_value
notify_next_itimer_func(nclient, which)
    Notify_client   nclient;
    int             which;
{
    Notify_func     func;
    NTFY_TYPE       type;

    /* Check arguments */
    if (ndet_check_which(which, &type))
	return (NOTIFY_UNEXPECTED);
    if ((func = nint_next_callout(nclient, type)) == NOTIFY_FUNC_NULL)
	return (NOTIFY_UNEXPECTED);
    return (func(nclient, which));
}
