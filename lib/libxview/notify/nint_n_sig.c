/*
 * Implements the nint n sig routines used by the XView notify module.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#include <xview_private/nint_n_sig_.h>
#include <xview_private/ndet_sig_.h>
#include <xview_private/nint_next_.h>
#include <xview_private/ndet.h>
#include <xview_private/nint.h>

extern          Notify_value
notify_next_signal_func(nclient, signal, mode)
    Notify_client   nclient;
    int             signal;
    Notify_signal_mode mode;
{
    Notify_func     func;
    NTFY_TYPE       type;

    /* Check arguments */
    if (ndet_check_mode(mode, &type) || ndet_check_sig(signal))
	return (NOTIFY_UNEXPECTED);
    if ((func = nint_next_callout(nclient, type)) == NOTIFY_FUNC_NULL)
	return (NOTIFY_UNEXPECTED);
    return (func(nclient, signal, mode));
}
