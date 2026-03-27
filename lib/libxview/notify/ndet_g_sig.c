/*
 * Implements the ndet g sig routines used by the XView notify module.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#include <xview_private/ndet_g_sig_.h>
#include <xview_private/ndetgetfnc_.h>
#include <xview_private/ndet_sig_.h>
#include <xview_private/ndet.h>

extern          Notify_func
notify_get_signal_func(nclient, signal, mode)
    Notify_client   nclient;
    int             signal;
    Notify_signal_mode mode;
{
    NTFY_TYPE       type;

    /* Check arguments */
    if (ndet_check_mode(mode, &type))
	return (NOTIFY_FUNC_NULL);
    if (ndet_check_sig(signal))
	return (NOTIFY_FUNC_NULL);
    return (ndet_get_func(nclient, type, (NTFY_DATA)(long)signal, NTFY_USE_DATA));
}
