/*
 * Implements the ndet s out routines used by the XView notify module.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#include <xview_private/ndet_s_out_.h>
#include <xview_private/ndet_s_fd_.h>
#include <xview_private/ndet.h>

extern          Notify_func
notify_set_output_func(nclient, func, fd)
    Notify_client   nclient;
    Notify_func     func;
    int             fd;
{
    return (ndet_set_fd_func(nclient, func, fd, NTFY_OUTPUT));
}
