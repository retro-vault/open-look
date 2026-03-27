/*
 * Implements the ndisd wait routines used by the XView notify module.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#include <xview_private/ndisd_wait_.h>
#include <xview_private/ndis.h>
#include <signal.h>

/* ARGSUSED */
extern          Notify_value
notify_default_wait3(client, pid, status, rusage)
    Notify_client   client;
    int             pid;
#if !defined(SVR4) && !defined(linux)
    union wait     *status;
#else /* SVR4 */
    int *status;
#endif /* SVR4 */
    struct rusage  *rusage;
{
    return (NOTIFY_IGNORED);
}
