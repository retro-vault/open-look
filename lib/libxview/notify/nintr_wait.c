/*
 * Implements the nintr wait routines used by the XView notify module.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#include <xview_private/nintr_wait_.h>
#include <xview_private/nintremove_.h>
#include <xview_private/ndet.h>
#include <xview_private/nint.h>

extern          Notify_error
notify_remove_wait3_func(nclient, func, pid)
    Notify_client   nclient;
    Notify_func     func;
    int             pid;
{
    /* Don't check pid because may be gone by now */
    return (nint_remove_func(nclient, func, NTFY_WAIT3,
			     (NTFY_DATA)(long)pid, NTFY_USE_DATA));
}
