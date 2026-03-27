/*
 * Implements the nint n fd routines used by the XView notify module.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#include <xview_private/nint_n_fd_.h>
#include <xview_private/ndet_fd_.h>
#include <xview_private/nint_next_.h>
#include <xview_private/ndet.h>
#include <xview_private/nint.h>

pkg_private     Notify_value
nint_next_fd_func(nclient, type, fd)
    Notify_client   nclient;
    NTFY_TYPE       type;
    int             fd;
{
    Notify_func     func;

    /* Check arguments */
    if (ndet_check_fd(fd))
	return (NOTIFY_UNEXPECTED);
    if ((func = nint_next_callout(nclient, type)) == NOTIFY_FUNC_NULL)
	return (NOTIFY_UNEXPECTED);
    return (func(nclient, fd));
}
