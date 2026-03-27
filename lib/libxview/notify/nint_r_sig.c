/*
 * Implements the nint r sig routines used by the XView notify module.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#include <xview_private/nint_r_sig_.h>
#include <xview_private/ndet_sig_.h>
#include <xview_private/nintremove_.h>
#include <xview_private/ndet.h>
#include <xview_private/nint.h>

extern          Notify_error
notify_remove_signal_func(nclient, func, signal, mode)
    Notify_client   nclient;
    Notify_func     func;
    int             signal;
    Notify_signal_mode mode;
{
    NTFY_TYPE       type;

    /* Check arguments */
    if (ndet_check_mode(mode, &type))
	return (notify_errno);
    if (ndet_check_sig(signal))
	return (notify_errno);
    return (nint_remove_func(nclient, func, type, (NTFY_DATA)(long)signal,
			     NTFY_USE_DATA));
}
