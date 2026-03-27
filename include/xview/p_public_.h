/*
 * Declares public XView interfaces, types, and attributes for p
 * public.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(P_PUBLIC__H)
#define P_PUBLIC__H

#include <xview/pkg.h>
#include <xview_private/panel_impl.h>

Sv1_public Event *panel_window_event(Panel client_panel, register Event *event);
Sv1_public Event *panel_event(Panel client_panel, register Event *event);

#endif
