/*
 * Implements the nint n fds routines used by the XView notify module.
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
#include <xview_private/nint.h>

extern          Notify_value
notify_next_fd_func(nclient, poll_fd)
    Notify_client   nclient;
    struct poll	   *poll_fd;
{
    return (nint_next_fds_func(nclient, NTFY_FD, poll_fd));
}

#endif /* POLL */
