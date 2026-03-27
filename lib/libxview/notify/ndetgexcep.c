/*
 * Implements the ndetgexcep routines used by the XView notify module.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#include <xview_private/ndetgexcep_.h>
#include <xview_private/ndet_g_fd_.h>
#include <xview_private/ndet.h>

extern          Notify_func
notify_get_exception_func(nclient, fd)
    Notify_client   nclient;
    int             fd;
{
    return (ndet_get_fd_func(nclient, fd, NTFY_EXCEPTION));
}
