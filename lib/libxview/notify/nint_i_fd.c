/*
 * Implements the nint i fd routines used by the XView notify module.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#include <xview_private/nint_i_fd_.h>
#include <xview_private/ndet_fd_.h>
#include <xview_private/nint_inter_.h>
#include <xview_private/ndet.h>
#include <xview_private/nint.h>

extern          Notify_error
nint_interpose_fd_func(nclient, func, type, fd)
    Notify_client   nclient;
    Notify_func     func;
    NTFY_TYPE       type;
    int             fd;
{
    /* Check arguments */
    if (ndet_check_fd(fd))
	return (notify_errno);
    return (nint_interpose_func(nclient, func, type, (NTFY_DATA)(long)fd,
				NTFY_USE_DATA));
}
