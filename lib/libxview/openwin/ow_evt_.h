/*
 * Declares internal interfaces used by the XView openwin module.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(OW_EVT__H)
#define OW_EVT__H

#include <xview/pkg.h>
#include <xview_private/ow_impl.h>

Pkg_private Notify_value openwin_event(Openwin owin_public, Event *event, Notify_arg arg, Notify_event_type type);
Pkg_private Notify_value openwin_view_event(Xv_Window window_public, Event *event, Notify_arg arg, Notify_event_type type);

#endif
