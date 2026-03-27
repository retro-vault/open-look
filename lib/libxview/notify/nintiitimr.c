/*
 * Implements the nintiitimr routines used by the XView notify module.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#include <xview_private/nintiitimr_.h>
#include <xview_private/ndetitimer_.h>
#include <xview_private/nint_inter_.h>
#include <xview_private/ndet.h>
#include <xview_private/nint.h>

extern          Notify_error
notify_interpose_itimer_func(nclient, func, which)
    Notify_client   nclient;
    Notify_func     func;
    int             which;
{
    NTFY_TYPE       type;

    /* Check arguments */
    if (ndet_check_which(which, &type))
	return (notify_errno);
    return (nint_interpose_func(nclient, func, type, NTFY_DATA_NULL,
				NTFY_IGNORE_DATA));
}
