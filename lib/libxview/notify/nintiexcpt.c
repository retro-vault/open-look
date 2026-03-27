/*
 * Implements the nintiexcpt routines used by the XView notify module.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#include <xview_private/nintiexcpt_.h>
#include <xview_private/nint_i_fd_.h>
#include <xview_private/nint.h>

extern          Notify_error
notify_interpose_exception_func(nclient, func, fd)
    Notify_client   nclient;
    Notify_func     func;
    int             fd;
{
    return (nint_interpose_fd_func(nclient, func, NTFY_EXCEPTION, fd));
}
