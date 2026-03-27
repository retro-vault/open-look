/*
 * Implements the nint copy routines used by the XView notify module.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#include <xview_private/nint_copy_.h>
#include <xview_private/ntfy_node_.h>
#include <xview_private/ndet.h>
#include <xview_private/nint.h>
#include <xview_private/portable.h>
#ifdef SVR4 
#include <stdlib.h> 
#endif /* SVR4 */

pkg_private     Notify_error
nint_copy_callout(new_cond, old_cond)
    NTFY_CONDITION *new_cond;
    NTFY_CONDITION *old_cond;
{
    if (old_cond->func_count > 1) {
	if ((new_cond->callout.functions = ntfy_alloc_functions()) ==
	    NTFY_FUNC_PTR_NULL)
	    return (notify_errno);
	XV_BCOPY((caddr_t) old_cond->callout.functions,
	      (caddr_t) new_cond->callout.functions, sizeof(NTFY_NODE));
    } else
	new_cond->callout.function = old_cond->callout.function;
    return (NOTIFY_OK);
}
