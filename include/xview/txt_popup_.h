/*
 * Declares public XView interfaces, types, and attributes for txt
 * popup.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(TXT_POPUP__H)
#define TXT_POPUP__H

#include <xview/pkg.h>
#include <xview/panel.h>
#include <xview_private/txt_impl.h>

Pkg_private void textsw_create_popup_frame(Textsw_view_handle view, int popup_type);
Pkg_private void textsw_get_and_set_selection(Frame popup_frame, Textsw_view_handle view, int popup_type);
Pkg_private void textsw_set_dir_str(int popup_type);
Pkg_private Textsw_view_handle text_view_frm_p_itm(Panel_item panel_item);
Pkg_private Xv_Window frame_from_panel_item(Panel_item panel_item);
Pkg_private Es_index textsw_get_selection(Textsw_view_handle view, Es_index *first, Es_index *last_plus_one, CHAR *selected_str, int max_str_len);
Pkg_private void textsw_set_pop_up_location(Frame frame_parent, Frame pop_up_frame);

#endif
