/*
 * Implements the nint n out routines used by the XView notify module.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#include <xview_private/nint_n_out_.h>
#include <xview_private/nint_n_fd_.h>
#include <xview_private/ndet.h>
#include <xview_private/nint.h>

extern          Notify_value
notify_next_output_func(nclient, fd)
    Notify_client   nclient;
    int             fd;
{
    return (nint_next_fd_func(nclient, NTFY_OUTPUT, fd));
}
