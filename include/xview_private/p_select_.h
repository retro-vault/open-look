/*
 * Declares private XView interfaces, types, and macros for p select.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(P_SELECT__H)
#define P_SELECT__H

#include <xview/pkg.h>
#include <xview_private/panel_impl.h>

Pkg_private int panel_duplicate_key_is_down(Panel_info *panel, Event *event);
Pkg_private int panel_event_is_xview_semantic(register Event *event);
Pkg_private int panel_erase_action(Event *event);
Pkg_private int panel_navigation_action(Event *event);
Pkg_private Notify_value panel_default_event(Panel p_public, register Event *event, Notify_arg arg);
Sv1_public void panel_handle_event(Panel_item client_object, Event *event);
Sv1_public void panel_begin_preview(Panel_item client_object, Event * event);
Sv1_public void panel_update_preview(Panel_item client_object, Event *event);
Sv1_public void panel_accept_preview(Panel_item client_object, Event *event);
Sv1_public void panel_cancel_preview(Panel_item client_object, Event *event);
Sv1_public void panel_accept_menu(Panel_item client_object, Event *event);
Sv1_public void panel_accept_key(Panel_item client_object, Event *event);
Sv1_public void panel_cancel(Panel_item client_object, Event *event);
Xv_public void panel_default_handle_event(Panel_item client_object, register Event *event);
Xv_private Notify_value panel_base_event_handler(Notify_client client, Notify_event event, Notify_arg arg, Notify_event_type type);

#endif
