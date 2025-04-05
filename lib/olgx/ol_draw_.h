#if !defined(OL_DRAW__H)
#define OL_DRAW__H

#include <olgx_impl.h>

int calc_add_ins(int width, short add_ins[STRING_SIZE]);
void olgx_draw_box(Graphics_info *info, Window win, int x, int y, int width, int height, int state, Bool fill_in);
void olgx_draw_choice_item(Graphics_info *info, Window win, int x, int y, int width, int height, void *label, int state);
void olgx_draw_drop_target(Graphics_info *info, Window win, void *label, int x, int y, int state);

#endif
