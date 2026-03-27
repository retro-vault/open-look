/*
 * Declares internal interfaces used by the XView panel module.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(P_EVENT__H)
#define P_EVENT__H

#include <xview/pkg.h>
#include <xview_private/panel_impl.h>

Pkg_private Notify_value panel_notify_panel_event(Xv_Window window, register Event *event, Notify_arg arg, Notify_event_type type);
Pkg_private Notify_value panel_notify_event(Xv_Window paint_window, register Event *event, Notify_arg arg, Notify_event_type type);
Pkg_private void panel_itimer_set(register Panel panel_public, struct itimerval timer);

#endif
