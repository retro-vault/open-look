/*
 * Declares internal interfaces used by the XView notice module.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(NOTICE_OL__H)
#define NOTICE_OL__H

#include <xview/pkg.h>
#include <xview_private/noticeimpl.h>

Pkg_private void notice_subframe_layout(Notice_info *notice, Bool do_msg, Bool do_butt);
Pkg_private int notice_center(Notice_info *notice);
Pkg_private void notice_get_notice_size(register notice_handle notice, struct rect *rect, int *buttons_width);
Pkg_private void notice_layout(notice_handle notice, struct rect *rect, int totalButWidth);
Pkg_private void notice_do_buttons(notice_handle notice, struct rect *rect, int starty, notice_buttons_handle this_button_only, int totalButWidth);
Pkg_private void notice_drawbox(Xv_Window pw, struct rect *rectp, int quadrant, int leftoff, int topoff);
Pkg_private int notice_determine_font(Xv_Window client_window, notice_handle notice);
Pkg_private void notice_draw_borders(Xv_window window, int x, int y, int width, int height, int is_toplevel_window);

#endif
