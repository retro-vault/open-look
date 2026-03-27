/*
 * Implements the ndet g fds routines used by the XView notify module.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#ifdef POLL

#include <xview_private/ntfy.h>
#include <xview_private/ndet.h>

extern          Notify_func
notify_get_fd_func(nclient, fd)
    Notify_client   nclient;
    int             fd;
{
    return (ndet_get_fd_func(nclient, fd, NTFY_FD));
}

#endif /* POLL */
