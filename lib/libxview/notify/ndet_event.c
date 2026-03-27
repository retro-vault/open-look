/*
 * Implements the ndet event routines used by the XView notify module.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#include <xview_private/ndet_event_.h>
#include <xview_private/ntfy_debug_.h>
#include <xview_private/ndet.h>

pkg_private int
ndet_check_when(when, type_ptr)
    Notify_event_type when;
    NTFY_TYPE      *type_ptr;
{
    NTFY_TYPE       type;

    switch (when) {
      case NOTIFY_SAFE:
	type = NTFY_SAFE_EVENT;
	break;
      case NOTIFY_IMMEDIATE:
	type = NTFY_IMMEDIATE_EVENT;
	break;
      default:
	ntfy_set_errno(NOTIFY_INVAL);
	return (-1);
    }
    if (type_ptr)
	*type_ptr = type;
    return (0);
}
