/*
 * Declares internal interfaces used by the XView notice module.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(NOTICE_PT__H)
#define NOTICE_PT__H

#include <xview/pkg.h>
#include <xview/window.h>
#include <xview/font.h>
#include <xview_private/noticeimpl.h>


Xv_public int _notice_prompt(Xv_Window client_window, Event *event, ...);
Pkg_private int notice_text_width(Xv_Font font, CHAR *str);
Pkg_private int notice_button_width(Xv_Font font, Graphics_info *ginfo, notice_buttons_handle button);
Pkg_private void notice_build_button(Xv_Window pw, int x, int y, notice_buttons_handle button, Graphics_info *ginfo, int three_d);
Pkg_private int notice_block_popup(Notice_info *notice);

#endif
