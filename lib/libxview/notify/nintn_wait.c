/*
 * Implements the nintn wait routines used by the XView notify module.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#include <xview_private/nintn_wait_.h>
#include <xview_private/nint_next_.h>
#include <xview_private/ndet.h>
#include <xview_private/nint.h>

extern          Notify_value
notify_next_wait3_func(nclient, pid, status, rusage)
    Notify_client   nclient;
    int             pid;
#if !defined(SVR4) && !defined(linux)
    union wait     *status;
#else /* SVR4 */
    int *status;
#endif /* SVR4 */
    struct rusage  *rusage;
{
    Notify_func     func;

    /* Don't check pid because may be exiting */
    if ((func = nint_next_callout(nclient, NTFY_WAIT3)) == NOTIFY_FUNC_NULL)
	return (NOTIFY_UNEXPECTED);
    return (func(nclient, pid, status, rusage));
}
