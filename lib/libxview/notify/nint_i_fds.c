/*
 * Implements the nint i fds routines used by the XView notify module.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#ifdef POLL

#include <xview_private/ntfy.h>
#include <xview_private/nint.h>

extern          Notify_error
notify_interpose_fd_func(nclient, func, fd)
    Notify_client   nclient;
    Notify_func     func;
    int		    fd;
{
    return (nint_interpose_fd_func(nclient, func, NTFY_FD, fd));
}

#endif /* POLL */
