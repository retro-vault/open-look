/*
 * Declares internal interfaces used by the XView panel module.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(P_PAINT__H)
#define P_PAINT__H

#include <xview/pkg.h>
#include <xview_private/panel_impl.h>

Pkg_private void panel_redisplay(Panel panel_public, Xv_Window pw, Rectlist *repaint_area);
Pkg_private void panel_display(register Panel_info *panel, Panel_setting flag);
Xv_public int panel_paint(Panel client_object, Panel_setting flag);
Pkg_private void panel_redisplay_item(register Item_info *ip, Panel_setting flag);
Pkg_private void panel_clear_item(register Item_info *ip);
Xv_public void panel_default_clear_item(Panel_item item_public);
Pkg_private void panel_paint_border(Panel panel_public, Panel_info *panel, Xv_Window pw);

#endif
