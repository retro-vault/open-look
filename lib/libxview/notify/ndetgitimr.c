/*
 * Implements the ndetgitimr routines used by the XView notify module.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#include <xview_private/ndetgitimr_.h>
#include <xview_private/ndetgetfnc_.h>
#include <xview_private/ndetitimer_.h>
#include <xview_private/ndet.h>

extern          Notify_func
notify_get_itimer_func(nclient, which)
    Notify_client   nclient;
    int             which;
{
    NTFY_TYPE       type;

    /* Check arguments */
    if (ndet_check_which(which, &type))
	return (NOTIFY_FUNC_NULL);
    return (ndet_get_func(nclient, type, NTFY_DATA_NULL, NTFY_IGNORE_DATA));
}
