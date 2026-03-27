/*
 * Implements the ndet wait routines used by the XView notify module.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#include <xview_private/ndet_wait_.h>
#include <xview_private/ntfy_debug_.h>
#include <xview_private/ndet.h>
#include <errno.h>

pkg_private int
ndet_check_pid(pid)
    int             pid;
{
    if (kill(pid, 0)) {
	ntfy_set_errno((errno == ESRCH) ? NOTIFY_SRCH : NOTIFY_INVAL);
	return (-1);
    }
    return (0);
}
