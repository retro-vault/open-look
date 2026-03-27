/*
 * Implements the p public routines used by the XView panel module.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#include <xview_private/p_public_.h>

/* utilities for event location translation */

/*
 * translate a panel-space event to a window-space event.
 */
Sv1_public Event *
panel_window_event(client_panel, event)
    Panel           client_panel;
    register Event *event;
{

    canvas_window_event(client_panel, event);
    return event;
}


/*
 * translate a window-space event to a panel-space event.
 */
Sv1_public Event *
panel_event(client_panel, event)
    Panel           client_panel;
    register Event *event;
{
    canvas_event(client_panel, event);
    return event;
}
