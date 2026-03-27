/*
 * Implements the ndet sig routines used by the XView notify module.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#include <xview_private/ndet_sig_.h>
#include <xview_private/ntfy_debug_.h>
#include <xview_private/ndet.h>
#include <signal.h>

pkg_private int
ndet_check_mode(mode, type_ptr)
    Notify_signal_mode mode;
    NTFY_TYPE      *type_ptr;
{
    NTFY_TYPE       type;

    switch (mode) {
      case NOTIFY_SYNC:
	type = NTFY_SYNC_SIGNAL;
	break;
      case NOTIFY_ASYNC:
	type = NTFY_ASYNC_SIGNAL;
	break;
      default:
	ntfy_set_errno(NOTIFY_INVAL);
	return (-1);
    }
    if (type_ptr)
	*type_ptr = type;
    return (0);
}

pkg_private int
ndet_check_sig(sig)
    int             sig;
{
    if (sig < 0 || sig >= NSIG) {
	ntfy_set_errno(NOTIFY_BAD_SIGNAL);
	return (-1);
    }
    return (0);
}
