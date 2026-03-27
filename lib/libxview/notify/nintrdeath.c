/*
 * Implements the nintrdeath routines used by the XView notify module.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#include <xview_private/nintrdeath_.h>
#include <xview_private/nintremove_.h>
#include <xview_private/ndet.h>
#include <xview_private/nint.h>

extern          Notify_error
notify_remove_destroy_func(nclient, func)
    Notify_client   nclient;
    Notify_func     func;
{
    return (nint_remove_func(nclient, func, NTFY_DESTROY, NTFY_DATA_NULL,
			     NTFY_IGNORE_DATA));
}
