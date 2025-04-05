#if !defined(OL_BUTTON__H)
#define OL_BUTTON__H

#include <olgx/olgx.h>
#include <olgx_impl.h>

void olgx_draw_button(Graphics_info *info, Window win, int x, int y, int width, int height, void *label, int state);
void olgx_draw_varheight_button(Graphics_info *info, Window win, int x, int y, int width, int height, int state);
void olgx_draw_menu_mark(Graphics_info *info, Window win, int x, int y, int state, int fill_in);
void olgx_draw_abbrev_button(Graphics_info *info, Window win, int x, int y, int state);
void olgx_stipple_rect(Graphics_info *info, Window win, int x, int y, int width, int height);
void olgx_draw_text(Graphics_info *info, Window win, char *string, int x, int y, int max_width, int state);
#ifdef OW_I18N
void olgx_draw_text(Graphics_info *info, Window win, void *string, int x, int y, int max_width, int state);
#else
void olgx_draw_text(Graphics_info *info, Window win, char *string, int x, int y, int max_width, int state);
#endif
void olgx_draw_pixmap_label(Graphics_info *info, Window win, Pixmap pix, int x, int y, int width, int height, int state);
void olgx_draw_ximage_label(Graphics_info *info, Window win, XImage *xim, int x, int y, int width, int height, int state);
void olgx_draw_textscroll_button(Graphics_info *info, Window win, int x, int y, int state);
void olgx_draw_numscroll_button(Graphics_info *info, Window win, int x, int y, int state);
void olgx_draw_accel_label(Graphics_info *info, Window win, int x, int y, int width, int height, void *main_label, int m_pos, void *qualifier_label, int q_pos, int mark_type, int mark_pos, void *key_label, int key_pos, void *background_pixmap, int state);
void olgx_draw_accel_button(Graphics_info *info, Window win, int x, int y, int width, int height, void *main_label, int m_pos, void *qualifier_label, int q_pos, int mark_type, int mark_pos, void *key_label, int key_pos, void *background_pixmap, int state);
void olgx_draw_accel_choice_item(Graphics_info *info, Window win, int x, int y, int width, int height, void *main_label, int m_pos, void *qualifier_label, int q_pos, int mark_type, int mark_pos, void *key_label, int key_pos, void *background_pixmap, int state);

#endif
