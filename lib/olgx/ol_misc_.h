#if !defined(OL_MISC__H)
#define OL_MISC__H

#include <olgx_impl.h>

void olgx_draw_resize_corner(Graphics_info *info, Window win, int x, int y, int type, int state);
void olgx_draw_pushpin(Graphics_info *info, Window win, int x, int y, int type);
void olgx_draw_check_box(Graphics_info *info, Window win, int x, int y, int state);
void olgx_draw_text_ledge(Graphics_info *info, Window win, int x, int y, int width);

#endif
