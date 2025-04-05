#if !defined(OL_SB__H)
#define OL_SB__H

#include <olgx_impl.h>

void olgx_draw_scrollbar(Graphics_info *ginfo, Window win, int x, int y, int length, int elev_pos, int old_elev_pos, int prop_pos, int prop_length, int state);
void olgx_draw_elevator(Graphics_info *info, Window win, int x, int y, int state);
void olgx_scroll_stipple_rects(Graphics_info *info, Window win, XRectangle *rects, int numrects);
XRectangle *olgx_compute_intersection(XRectangle *rect1, XRectangle *rect2);

#endif
