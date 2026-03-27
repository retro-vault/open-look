/*
 * Implements the ndet s fds routines used by the XView notify module.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#include <xview_private/ntfy.h>
#include <xview_private/ndet.h>

extern          Notify_func
notify_set_input_func(nclient, func, fd)
    Notify_client   nclient;
    Notify_func     func;
    struct poll     fd;
{
    return (ndet_set_poll_func(nclient, func, fd, NTFY_FD));
}
