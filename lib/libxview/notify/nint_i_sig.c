/*
 * Implements the nint i sig routines used by the XView notify module.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#include <xview_private/nint_i_sig_.h>
#include <xview_private/ndet_sig_.h>
#include <xview_private/nint_inter_.h>
#include <xview_private/nint_stack_.h>
#include <xview_private/ndet.h>
#include <xview_private/nint.h>

extern          Notify_error
notify_interpose_signal_func(nclient, func, signal, mode)
    Notify_client   nclient;
    Notify_func     func;
    int             signal;
    Notify_signal_mode mode;
{
    NTFY_TYPE       type;

    /*
     * Check arguments & pre-allocate stack incase going to get asynchronous
     * event before synchronous one.
     */
    if (ndet_check_mode(mode, &type) || ndet_check_sig(signal) ||
	(nint_alloc_stack() != NOTIFY_OK))
	return (notify_errno);
    return (nint_interpose_func(nclient, func, type, (NTFY_DATA)(long)signal,
				NTFY_USE_DATA));
}
