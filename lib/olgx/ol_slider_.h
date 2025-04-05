#if !defined(OL_SLIDER__H)
#define OL_SLIDER__H

#include <olgx_impl.h>

void olgx_draw_slider(Graphics_info *info, Window win, int x, int y, int width, int oldval, int newval, int state);
void olgx_draw_horizontal_slider(Graphics_info *info, Window win, int x, int y, int width, int value, int state);
void olgx_draw_slider_control(Graphics_info *info, Window win, int x, int y, int state);
void olgx_update_horizontal_slider(Graphics_info *info, Window win, int x, int y, int width, int old_value, int new_value, int state);
void olgx_update_vertical_slider(Graphics_info *info, Window win, int x, int y, int height, int old_value, int new_value, int state);
void olgx_draw_vertical_slider(Graphics_info *info, Window win, int x, int y, int height, int value, int state);
void olgx_draw_gauge(Graphics_info *info, Window win, int x, int y, int width, int oldval, int newval, int state);
void olgx_draw_horiz_gauge(Graphics_info *info, Window win, int x, int y, int width, int value);
void olgx_update_horiz_gauge(Graphics_info *info, Window win, int x, int y, int oldval, int newval);
void olgx_draw_vertical_gauge(Graphics_info *info, Window win, int x, int y, int width, int value);
void olgx_update_vertical_gauge(Graphics_info *info, Window win, int x, int y, int width, int oldval, int newval);

#endif
