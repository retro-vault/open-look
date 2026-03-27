/*
 * Implements the ndetgevent routines used by the XView notify module.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#include <xview_private/ndetgevent_.h>
#include <xview_private/ndet_event_.h>
#include <xview_private/ndetgetfnc_.h>
#include <xview_private/ndet.h>

extern          Notify_func
notify_get_event_func(nclient, when)
    Notify_client   nclient;
    Notify_event_type when;
{
    NTFY_TYPE       type;

    /* Check arguments */
    if (ndet_check_when(when, &type))
	return (NOTIFY_FUNC_NULL);
    return (ndet_get_func(nclient, type, NTFY_DATA_NULL, NTFY_IGNORE_DATA));
}
