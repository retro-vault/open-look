/*
 * Implements the nint set routines used by the XView notify module.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#include <xview_private/nint_set_.h>
#include <xview_private/ndet.h>
#include <xview_private/nint.h>

pkg_private     Notify_func
nint_set_func(cond, new_func)
    register NTFY_CONDITION *cond;
    Notify_func     new_func;
{
    Notify_func     old_func;

    if (cond->func_count > 1) {
	old_func = cond->callout.functions[cond->func_count - 1];
	cond->callout.functions[cond->func_count - 1] = new_func;
    } else {
	old_func = cond->callout.function;
	cond->callout.function = new_func;
	cond->func_count = 1;
    }
    return (old_func);
}
