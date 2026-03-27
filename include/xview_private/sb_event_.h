/*
 * Declares private XView interfaces, types, and macros for sb event.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(SB_EVENT__H)
#define SB_EVENT__H

#include <xview/pkg.h>
#include <xview_private/sb_impl.h>

Pkg_private Notify_value scrollbar_handle_event(Xv_Window sb_public, Event *event, Notify_arg arg, Notify_event_type type);
Pkg_private void scrollbar_line_to_top(Menu menu, Menu_item item);
Pkg_private void scrollbar_top_to_line(Menu menu, Menu_item item);
Pkg_private Menu scrollbar_gen_menu(Menu menu, Menu_generate op);
Pkg_private void scrollbar_split_view_from_menu(Menu menu, Menu_item item);
Pkg_private void scrollbar_join_view_from_menu(Menu menu, Menu_item item);
Pkg_private void scrollbar_last_position(Menu menu, Menu_item item);
Pkg_private void scrollbar_init_delay_values(int delay_time, int page_interval, int line_interval);
Pkg_private int scrollbar_left_mouse_up(Xv_scrollbar_info *sb);

#endif
