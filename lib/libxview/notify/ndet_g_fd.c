/*
 * Implements the ndet g fd routines used by the XView notify module.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#include <xview_private/ndet_g_fd_.h>
#include <xview_private/ndetgetfnc_.h>
#include <xview_private/ndet_fd_.h>
#include <xview_private/ndet.h>

extern          Notify_func
ndet_get_fd_func(nclient, fd, type)
    Notify_client   nclient;
    int             fd;
    NTFY_TYPE       type;
{
    /* Check arguments */
    if (ndet_check_fd(fd))
	return (NOTIFY_FUNC_NULL);
    return (ndet_get_func(nclient, type, (NTFY_DATA)(long)fd, NTFY_USE_DATA));
}
