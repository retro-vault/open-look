/*
 * Implements the ndetgdeath routines used by the XView notify module.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#include <xview_private/ndetgdeath_.h>
#include <xview_private/ndetgetfnc_.h>
#include <xview_private/ndet.h>

extern          Notify_func
notify_get_destroy_func(nclient)
    Notify_client   nclient;
{
    return (ndet_get_func(nclient, NTFY_DESTROY, NTFY_DATA_NULL,
			  NTFY_IGNORE_DATA));
}
