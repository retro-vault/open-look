/*
 * Declares public XView interfaces, types, and attributes for p btn.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(P_BTN__H)
#define P_BTN__H

#include <xview/pkg.h>
#include <xview/openmenu.h>
#include <xview_private/panel_impl.h>

Pkg_private int panel_button_init(Panel panel_public, Panel_item item_public, Attr_avlist avlist);
Pkg_private int panel_button_destroy(Panel_item item_public,Destroy_status  status);
Pkg_private void panel_btn_accepted(Item_info *ip, Event *event);

static void btn_begin_preview(Panel_item item_public, Event *event);
static void btn_cancel_preview(Panel_item item_public, Event *event);
static void btn_accept_preview(Panel_item item_public, Event *event);
static void btn_accept_menu(Panel_item item_public, Event *event);
static void btn_accept_key(Panel_item item_public, Event *event);
static void btn_paint(Panel_item item_public);
static void btn_remove(Panel_item item_public);
static void btn_accept_kbd_focus(Panel_item item_public);
static void btn_yield_kbd_focus(Panel_item item_public);
static void button_menu_busy_proc(Menu menu);
static void button_menu_done_proc(Menu menu, Xv_opaque result);
static Menu generate_menu(Menu menu);
static void panel_paint_button_image(Item_info *ip, Panel_image *image, int inactive_button, Xv_opaque menu, int height);
static void take_down_cmd_frame(Panel panel_public);

#endif
