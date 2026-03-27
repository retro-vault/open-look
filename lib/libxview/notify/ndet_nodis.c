/*
 * Implements the ndet nodis routines used by the XView notify module.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#include <xview_private/ndet_nodis_.h>
#include <xview_private/ndet.h>

extern          Notify_error
notify_no_dispatch()
{
    ndet_flags &= ~NDET_DISPATCH;
    return (NOTIFY_OK);
}
