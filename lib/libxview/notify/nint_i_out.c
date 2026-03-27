/*
 * Implements the nint i out routines used by the XView notify module.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#include <xview_private/nint_i_out_.h>
#include <xview_private/nint_i_fd_.h>
#include <xview_private/nint.h>

extern          Notify_error
notify_interpose_output_func(nclient, func, fd)
    Notify_client   nclient;
    Notify_func     func;
    int             fd;
{
    return (nint_interpose_fd_func(nclient, func, NTFY_OUTPUT, fd));
}
