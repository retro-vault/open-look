/*
 * Declares public XView interfaces, types, and attributes for p txt.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(P_TXT__H)
#define P_TXT__H

#include <xview/pkg.h>
#include <xview_private/panel_impl.h>

Pkg_private int text_init(Panel panel_public, Panel_item item_public, Attr_avlist avlist);
Pkg_private Xv_opaque text_set_avlist(Panel_item item_public, register Attr_avlist avlist);
Pkg_private Xv_opaque text_get_attr(Panel_item item_public, int *status, register Attr_attribute which_attr, va_list avlist);
Pkg_private int text_destroy(Panel_item item_public, Destroy_status status);
Pkg_private int panel_printable_char(int code);
Pkg_private void panel_text_caret_on(Panel_info *panel, int on);
Xv_public Panel_setting panel_text_notify(Panel_item client_item, register Event *event);
Pkg_private void panel_text_paint_label(register Item_info *ip);
#ifdef OW_I18N
Xv_private void ml_panel_display_interm(Item_info *ip);
Pkg_private void ml_panel_saved_caret(Item_info *ip);
Pkg_private void panel_implicit_commit(Item_info *ip);
#endif 

#endif
