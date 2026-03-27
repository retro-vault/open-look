/*
 * Implements the ndet veto routines used by the XView notify module.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#include <xview_private/ndet_veto_.h>
#include <xview_private/ntfyprotec_.h>
#include <xview_private/ndet.h>

/* ARGSUSED */
extern          Notify_error
notify_veto_destroy(nclient)
    Notify_client   nclient;
{
    NTFY_BEGIN_CRITICAL;
    ndet_flags |= NDET_VETOED;
    NTFY_END_CRITICAL;
    return (NOTIFY_OK);
}
