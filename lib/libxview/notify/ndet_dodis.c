/*
 * Implements the ndet dodis routines used by the XView notify module.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#include <xview_private/ndet_dodis_.h>
#include <xview_private/ndet.h>

/* If this is set to a valid fd, exclude that fd from implicit dispatching */
int notify_exclude_fd = -5;

extern          Notify_error
notify_do_dispatch()
{
    ndet_flags |= NDET_DISPATCH;
    return (NOTIFY_OK);
}
