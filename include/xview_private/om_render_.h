/*
 * Declares private XView interfaces, types, and macros for om render.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(OM_RENDER__H)
#define OM_RENDER__H

#include <xview/pkg.h>
#include <xview/window.h>
#include <xview_private/om_impl.h>

Pkg_private void menu_render(Xv_menu_info *menu, Xv_menu_group_info *group, Xv_menu_item_info *parent);
Pkg_private int compute_item_size(Xv_menu_info *menu, struct image *std_image, int *status, int call_gen_proc);
void menu_window_event_proc(Xv_Window window, Event *event);
void menu_shadow_event_proc(Xv_Window window, Event *event);
Pkg_private Notify_value menu_client_window_event_proc(Xv_Window win, Event *event, Notify_arg arg, Notify_event_type type);

#endif
