/*
 * Implements the icon input routines used by the XView icon module.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#include <xview_private/icon_input_.h>
#include <stdio.h>
#include <xview/xview.h>

/* ARGSUSED */
Notify_value
icon_input(icon_public, event, arg, type)
    Icon            icon_public;
    Event          *event;
    Notify_arg      arg;
    Notify_event_type type;
{
    switch (event_action(event)) {

      case WIN_REPAINT:
	    icon_display(icon_public, 0, 0);
	    return (NOTIFY_DONE);
	    break;

      default:
	return (NOTIFY_IGNORED);
	break;
    }
}
