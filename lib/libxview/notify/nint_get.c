/*
 * Implements the nint get routines used by the XView notify module.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#include <xview_private/nint_get_.h>
#include <xview_private/ndet.h>
#include <xview_private/nint.h>

pkg_private     Notify_func
nint_get_func(cond)
    register NTFY_CONDITION *cond;
{
    Notify_func     func;

    if (cond->func_count > 1)
	func = cond->callout.functions[cond->func_count - 1];
    else
	func = cond->callout.function;
    return (func);
}
