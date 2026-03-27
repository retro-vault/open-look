/*
 * Implements the ndetg wait routines used by the XView notify module.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#include <xview_private/ndetg_wait_.h>
#include <xview_private/ndetgetfnc_.h>
#include <xview_private/ndet_wait_.h>
#include <xview_private/ndet.h>

extern          Notify_func
notify_get_wait3_func(nclient, pid)
    Notify_client   nclient;
    int             pid;
{
    /* Check arguments */
    if (ndet_check_pid(pid))
	return (NOTIFY_FUNC_NULL);
    return (ndet_get_func(nclient, NTFY_WAIT3,
			  (NTFY_DATA)(long)pid, NTFY_USE_DATA));
}
