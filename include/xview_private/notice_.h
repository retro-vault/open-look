/*
 * Declares private XView interfaces, types, and macros for notice.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(NOTICE__H)
#define NOTICE__H

#include <xview/pkg.h>
#include <xview/notice.h>
#include <xview/panel.h>
#include <xview_private/noticeimpl.h>


Pkg_private int notice_init_internal(Xv_Window client_window, Xv_notice_struct *notice_public, Attr_avlist avlist);
Pkg_private void notice_add_default_button(register notice_handle notice);
Pkg_private void notice_defaults(notice_handle notice);
Pkg_private void notice_add_button_to_list(register notice_handle notice, notice_buttons_handle button);
Pkg_private void notice_add_msg_to_list(register notice_handle notice, notice_msgs_handle msg);
Pkg_private notice_buttons_handle notice_create_button_struct(void);
Pkg_private notice_msgs_handle notice_create_msg_struct(void);
Pkg_private void notice_free_msg_structs(notice_msgs_handle first);
Pkg_private void notice_free_button_structs(notice_buttons_handle first);
Pkg_private void notice_do_bell(notice_handle notice);
Pkg_private void notice_update_xy(Notice_info *notice);
Pkg_private void notice_button_panel_proc(Panel_item item, Event *event);
Pkg_private int notice_get_owner_frame(Notice_info *notice);

#endif
