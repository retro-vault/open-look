/*
 * Declares internal interfaces used by the XView window module.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(WINDOWUTIL__H)
#define WINDOWUTIL__H

#include <xview/pkg.h>
#include <xview_private/windowimpl.h>

void window_release_event_lock(Xv_Window window);
void window_refuse_kbd_focus(Xv_Window window);
int window_read_event(Xv_Window window, Event *event);
void window_bell(Xv_Window window);
Pkg_private int win_appeal_to_owner(int adjust, register Window_info *win, Xv_opaque op, Xv_opaque d1, Xv_opaque d2);
Pkg_private Notify_value window_default_event_func(Xv_Window win_public, Event *event, Notify_arg arg, Notify_event_type type);
Xv_private GC window_private_gc(Xv_Window win_public);
Xv_private int window_getrelrect(Xv_object dest_child, Xv_object source, register Rect *source_rect);
Xv_public int xv_rows(Xv_Window win_public, int rows);
Xv_public int xv_row(Xv_Window win_public, int row);
Xv_public int xv_cols(Xv_Window win_public, int cols);
Xv_public int xv_col(Xv_Window win_public, int col);
Xv_private void window_get_outer_rect(Xv_Window window, Rect *rect);
Xv_private void window_set_outer_rect(Xv_Window window, Rect *rect);
Xv_private void window_set_client_message(Xv_Window window, XClientMessageEvent *msg);
Xv_private void window_get_cache_rect(Xv_Window window, Rect *rect);
Xv_private void window_update_cache_rect(Xv_Window window, Rect *rect);
Xv_private void window_set_cache_rect(Xv_Window window, Rect *rect);
Xv_private void window_set_parent(Xv_Window window, Xv_Window parent);
Xv_private void window_set_map_cache(Xv_Window window, int flag);
Xv_private int window_get_map_cache(Xv_Window window);
Xv_private void window_set_border(Xv_object window, int width);
Xv_private void window_outer_to_innerrect(Xv_Window window, Rect *rect);
Xv_private void window_inner_to_outerrect(Xv_Window window, Rect *rect);
Xv_private void window_set_rescale_state(Xv_Window window, int state);
Xv_private int window_get_rescale_state(Xv_Window window);
Xv_private void window_end_rescaling(Xv_Window window);
Xv_private void window_start_rescaling(Xv_Window window);
Xv_private void window_calculate_new_size(Xv_Window parent, Xv_Window window, int *new_width, int *new_height);
Xv_private void window_destroy_rect_obj_list(Window_rescale_rect_obj *rect_obj_list);
Xv_private Window_rescale_rect_obj *window_create_rect_obj_list(int num_elems);
Xv_private void window_add_to_rect_list(Window_rescale_rect_obj *rect_obj_list, Xv_Window window, Rect *rect, int i);
Xv_private int window_rect_equal_ith_obj(Window_rescale_rect_obj *rect_obj_list, Rect *rect, int i);
Xv_private void window_adjust_rects(Window_rescale_rect_obj *rect_obj_list, Xv_Window parent_public, int num_elems, int parent_width, int parent_height);
Xv_private int xv_get_external_data(Xv_object window, char *key, Xv_opaque *data, int *len, int *format);
Xv_private int xv_write_external_data(Xv_object window, char *key, int format, Xv_opaque *data, int len);
Xv_public int xv_send_message(Xv_object window, Xv_opaque addresse, char *msg_type, int format, Xv_opaque *data, int len);
Xv_private void win_grab_quick_sel_keys(Xv_Window window);
Xv_private void win_ungrab_quick_sel_keys(Xv_Window window);
Xv_private void win_set_wm_command(Xv_Window window);
Xv_private void win_set_wm_class(Xv_Window window);
Xv_private int window_set_tree_flag(Xv_Window topLevel, Xv_cursor pointer, int deafBit, Bool flag);
Xv_private int window_set_tree_child_flag(Xv_window query, Xv_cursor pointer, int deafBit, Bool flag);
Xv_private int window_set_flag_cursor(Xv_window window, Xv_cursor pointer, Bool flag);
#ifdef OW_I18N
Xv_private int window_set_ic_focus_win(Xv_window window, XIC ic, XID focus_win);
Xv_private int window_set_xungrabkeyboard(Xv_window window, Display *display, Time time);
Xv_private int window_set_xgrabkeyboard(Xv_window window, Display *dpy, grab_window, Bool owner_events, int ptr_mode, int kbd_mode, Time time);
#endif

#endif
