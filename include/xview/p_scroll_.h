/*
 * Declares public XView interfaces, types, and attributes for p
 * scroll.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(P_SCROLL__H)
#define P_SCROLL__H

#include <xview/pkg.h>
#include <xview_private/panel_impl.h>
#include <xview/scrollbar.h>

Pkg_private int panel_normalize_scroll(Scrollbar sb, long unsigned offset, Scroll_motion motion, long unsigned *vs);
Pkg_private void panel_update_scrolling_size(Panel client_panel);

static int top_pair(Panel_info *panel, int target, Item_info **low_ip, Item_info **high_ip);
static void normalize_top(Panel_info *panel, int *offset);
static void normalize_bottom(Panel_info *panel, Xv_Window pw, int scrolling_up, int *offset);
static int left_pair(Panel_info *panel, int target, Item_info **low_ip, Item_info **high_ip);
static void normalize_left(Panel_info *panel, int *offset);
static void normalize_right(Panel_info *panel, Xv_Window pw, int scrolling_up, int *offset);



#endif
