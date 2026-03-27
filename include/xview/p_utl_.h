/*
 * Declares public XView interfaces, types, and attributes for p utl.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(P_UTL__H)
#define P_UTL__H

#include <xview/pkg.h>
#include <xview_private/panel_impl.h>

Pkg_private Rect panel_enclosing_rect(register Rect *r1, register Rect *r2);
Pkg_private void panel_update_extent(Panel_info *panel, Rect rect);
Pkg_private int panel_col_to_x(Xv_Font font, int col);
Pkg_private int panel_row_to_y(Xv_Font font, int line);
Pkg_private int panel_x_to_col(Xv_Font font, int x);
Pkg_private int panel_y_to_row(Xv_Font font, int y);
Pkg_private struct pr_size panel_make_image(Xv_Font font, Panel_image *dest, int type_code, Xv_opaque value, int bold_desired, int inverted_desired);
Pkg_private void panel_image_set_font(Panel_image *image, Xv_Font font);
Pkg_private Item_info *panel_successor(register Item_info *ip);
Pkg_private void panel_append(register Item_info *ip);
Pkg_private void panel_unlink(register Item_info *ip, int destroy);
Pkg_private void panel_find_default_xy(Panel_info *panel, Item_info *item);
Pkg_private void panel_item_layout(register Item_info *ip, register Rect *deltas);
Pkg_private void panel_check_item_layout(Item_info *ip);
Xv_public void panel_paint_label(Panel_item item_public);
Pkg_private void panel_paint_image(Panel_info *panel, Panel_image *image, Rect *rect, int inactive_item, int color_index);
Pkg_private void panel_invert(Panel_info *panel, register Rect *r, int color_index);
Pkg_private void panel_pw_invert(Xv_Window pw, register Rect *rect, int color_index);
Pkg_private char *panel_strsave(char *source);
#ifdef OW_I18N
Pkg_private wchar_t *panel_strsave_wc(wchar_t *source);
#endif 
Pkg_private int panel_fonthome(Xv_Font font);
int panel_nullproc(void);
Pkg_private void panel_free_choices(Panel_image *choices, int first, int last);
Pkg_private void panel_free_image(Panel_image *image);
Pkg_private void panel_set_bold_label_font(register Item_info *ip);
#ifdef OW_I18N
Pkg_private void panel_paint_text(Xv_opaque pw, XFontSet font_xid, int color_index, int x, int y, CHAR *str);
#else
Pkg_private void panel_paint_text(Xv_opaque pw, Font font_xid, int color_index, int x, int y, CHAR *str);
#endif 
Pkg_private void panel_paint_svrim(Xv_Window pw, Pixrect *pr, int x, int y, int color_index, Pixrect *mask_pr);
Pkg_private Panel_item panel_set_kbd_focus(Panel_info *panel, Item_info *ip);
Xv_public Panel_item panel_advance_caret(Panel panel_public);
Xv_public Panel_item panel_backup_caret(Panel panel_public);
Pkg_private Item_info *panel_next_kbd_focus(Panel_info *panel, int wrap);
Pkg_private Item_info *panel_previous_kbd_focus(Panel_info *panel, int wrap);
Pkg_private void panel_accept_kbd_focus(Panel_info *panel);
Pkg_private void panel_yield_kbd_focus(Panel_info *panel);
Pkg_private void panel_clear_pw_rect(Xv_window pw, Rect rect);
Pkg_private void panel_clear_rect(Panel_info *panel, Rect rect);
Pkg_private Rect *panel_viewable_rect(Panel_info *panel, Xv_Window pw);
Pkg_private int panel_viewable_height(Panel_info *panel, Xv_Window pw);
Pkg_private int panel_viewable_width(Panel_info *panel, Xv_Window pw);
Pkg_private int panel_height(Panel_info *panel);
Pkg_private int panel_width(Panel_info *panel);
Pkg_private int panel_is_multiclick(Panel_info *panel, struct timeval *last_click_time, struct timeval *new_click_time);
Pkg_private void panel_user_error(Item_info *object, Event *event);
Xv_public void panel_show_focus_win(Panel_item item_public, Frame frame, int x, int y);
Pkg_private int panel_round(register int x, register int y);
Pkg_private int panel_wants_focus(Panel_info *panel); 
Pkg_private void panel_autoscroll_start_itimer(Panel_item item, int *autoscroll_itimer_func);
Pkg_private void panel_autoscroll_stop_itimer(Panel_item item);

#endif
