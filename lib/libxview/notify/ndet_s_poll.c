/*
 * Implements the ndet s poll routines used by the XView notify module.
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

#include <poll.h>

extern          Notify_func
notify_set_fd_func(nclient, func, poll_fd)
    Notify_client   nclient;
    Notify_func     func;
    struct poll    *poll_fd;
{
    return (ndet_set_fd_func(nclient, func, poll_fd, NTFY_FD));
}
#endif /* POLL */
