/*
 * Declares internal interfaces used by the XView textsw module.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(EV_DISPLAY__H)
#define EV_DISPLAY__H

#include <xview/pkg.h>
#include <xview/window.h>
#include <xview_private/ev_impl.h>

Pkg_private void ev_note_esh_modified(register Ev_chain views, Es_index first, Es_index last_plus_one, int by);
Pkg_private int ev_check_cached_pos_info(register Ev_handle view, register Es_index pos, register Ev_pos_info *cache);
Pkg_private int ev_insert_to_xy(register Ev_handle view, int *x, int *y);
#ifdef OW_I18N
#ifdef FULL_R5
Pkg_private int ev_caret_to_xy(register Ev_handle view, int *x, int *y);
#endif 
#endif 
Pkg_private void ev_check_insert_visibility(register Ev_chain views);
Pkg_private void ev_blink_caret(Xv_Window focus_view, register Ev_chain views, int on);
Pkg_private void ev_clear_rect(Ev_handle view, register struct rect *rect);
#ifdef DEBUG
Pkg_private void ev_debug_clear_rect(Ev_handle view, register struct rect *rect);
#endif
Pkg_private void ev_clear_to_bottom(register Ev_handle view, register Rect *rect, int new_top, unsigned clear_current_first);
Pkg_private Ev_line_table ev_ft_for_rect(Ei_handle eih, struct rect *rect);
Pkg_private void ev_clear_selection(Ev_chain chain, unsigned type);
Pkg_private void ev_view_range(Ev_handle view, Es_index *first, Es_index *last_plus_one);
Pkg_private int ev_xy_in_view(register Ev_handle view, register Es_index pos, register int *lt_index, register Rect *rect);
Pkg_private void ev_display_range(Ev_chain chain, Es_index first, Es_index last_plus_one);
Pkg_private void ev_set_selection(Ev_chain chain, register Es_index first, register Es_index last_plus_one, unsigned type);
Pkg_private int ev_get_selection(Ev_chain chain, Es_index *first, Es_index *last_plus_one, unsigned type);
Pkg_private void ev_display_view(Ev_handle view);
Pkg_private void ev_display_views(Ev_chain chain);
Pkg_private void ev_clear_from_margins(register Ev_handle view, register Rect *from_rect, register Rect *to_rect);
Pkg_private void ev_set_start(Ev_handle view, Es_index start);
Pkg_private void ev_add_margins(Ev_handle view, register Rect *rect);
Pkg_private void ev_display_fixup(register Ev_handle view);
Pkg_private Es_index ev_scroll_lines(register Ev_handle view, int line_count, int scroll_by_display_lines);
Pkg_private void ev_display_in_rect(register Ev_handle view, register Rect *rect);
Pkg_private int ev_fill_esbuf(Es_buf_handle esbuf, Es_index *ptr_to_last_plus_one);
Pkg_private void ev_range_info(Ev_line_table op_bdry, Es_index pos, register struct range *range);
Pkg_private Ev_process_handle ev_process_init(register Ev_process_handle ph, Ev_handle view, Es_index first, Es_index stop_plus_one, Rect *rect, CHAR *buf, int sizeof_buf);
Pkg_private int ev_process_update_buf(register Ev_process_handle ph);
Pkg_private unsigned long ev_process(register Ev_process_handle ph, long unsigned flags, int op, int rop, Xv_Window pw);
Pkg_private struct ei_process_result ev_ei_process(register Ev_handle view, Es_index start, Es_index stop_plus_one);
Pkg_private struct ei_process_result ev_line_lpo(Ev_handle view, Es_index line_start);
Pkg_private Es_index ev_display_line_start(register Ev_handle view, Es_index pos);
Pkg_private void ev_do_glyph(register Ev_handle view, Es_index *glyph_pos_ptr, Ev_overlay_handle *glyph_ptr, struct ei_process_result *result);
Pkg_private struct ei_process_result ev_display_internal(register Ev_handle view, register Rect *rect, register int line, register Es_index stop_plus_one, int ei_op, int break_action);
Pkg_private Ev_expand_status ev_expand(register Ev_handle view, Es_index start, Es_index stop_plus_one, CHAR *out_buf, int out_buf_len, int *total_chars);
Pkg_private int ev_line_for_y(Ev_handle view, int y);
Pkg_private Es_index ev_index_for_line(Ev_handle view, int line);
Pkg_private Es_index ev_considered_for_line(Ev_handle view, int line);
Pkg_private struct rect ev_rect_for_line(Ev_handle view, int line);
Pkg_private void ev_extend_to_view_bottom(Ev_handle view, struct rect *rect);
Pkg_private Ev_handle ev_resolve_xy_to_view(Ev_chain views, register int x, register int y);
Pkg_private Ev_handle ev_nearest_view(Ev_chain views, register int x, register int y, register int *x_used, register int *y_used);
Pkg_private Es_index ev_resolve_xy(Ev_handle view, int x, int y);
#ifdef OW_I18N
Pkg_private void ev_set_pre_edit_region(Ev_chain chain, register Es_index first, register Es_index last_plus_one, unsigned type);
#endif 
Pkg_private void ev_paint_view(Ev_handle e_view, Xv_window xv_win, XEvent *xevent);

#endif
