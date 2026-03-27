/*
 * Declares internal interfaces used by the XView menu module.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(OM_PUBLIC__H)
#define OM_PUBLIC__H

#include <xview/pkg.h>
#include <xview/openmenu.h>
#include <xview/panel.h>
#include <xview_private/om_impl.h>
#include <xview_private/fm_impl.h>

Xv_public void _menu_show(Menu menu_public, Xv_Window win, struct inputevent *iep, ...);
Pkg_private void menu_done(register Xv_menu_info *m);
Pkg_private int menu_create_internal(Xv_opaque parent, Xv_opaque object, Xv_opaque *avlist);
Pkg_private int menu_create_item_internal(Xv_opaque parent, Xv_opaque object, Xv_opaque *avlist);
Pkg_private int menu_destroy_internal(Menu menu_public, Destroy_status status);
Pkg_private int menu_item_destroy_internal(Menu_item menu_item_public, Destroy_status status);
Pkg_private Xv_opaque menu_pullright_return_result(Menu_item menu_item_public);
Xv_public Xv_opaque menu_return_value(Menu menu_public, Menu_item menu_item_public);
Xv_public Xv_opaque menu_return_item(Menu menu_public, Menu_item menu_item_public);
Pkg_private void menu_return_no_value(Menu menu_public);
Sv1_public Menu_item _menu_find(Menu menu_public, ...);
Pkg_private Xv_opaque menu_pkg_find(Menu menu_public, Xv_pkg *pkg, Attr_attribute avlist[ATTR_STANDARD_SIZE]);
Xv_private void menu_select_default(Menu menu_public);
Xv_private void menu_return_default(Menu menu_public, int depth, Event *event);
Xv_public void menu_default_pin_proc(Menu menu_public, int x, int y);
Pkg_private void menu_create_pin_panel_items(Panel panel, Xv_menu_info *menu);
Xv_private void menu_save_pin_window_rect(Xv_Window win);
Pkg_private Notify_value menu_pin_window_event_proc(Xv_Window win, Event *event, Notify_arg arg, Notify_event_type type);
Xv_private void menu_item_set_parent(Menu_item menu_item_public, Menu menu_public);
Xv_private void menu_accelerator_notify_proc(Frame_accel_data *accelerator_data, Event *event);

#endif
