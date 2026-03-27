/*
 * Implements the nintievent routines used by the XView notify module.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#include <xview_private/nintievent_.h>
#include <xview_private/ndet_event_.h>
#include <xview_private/nint_inter_.h>
#include <xview_private/ndet.h>
#include <xview_private/nint.h>

extern          Notify_error
notify_interpose_event_func(nclient, func, when)
    Notify_client   nclient;
    Notify_func     func;
    Notify_event_type when;
{
    NTFY_TYPE       type;

    /* Check arguments */
    if (ndet_check_when(when, &type))
	return (notify_errno);
    return (nint_interpose_func(nclient, func, type, NTFY_DATA_NULL,
				NTFY_IGNORE_DATA));
}
