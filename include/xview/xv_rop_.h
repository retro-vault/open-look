/*
 * Declares public XView interfaces, types, and attributes for xv rop.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(XV_ROP__H)
#define XV_ROP__H

#include <xview/pkg.h>
#include <X11/Xresource.h>
#include <xview_private/draw_impl.h>
#include <pixrect/pixrect.h>
#include <xview/pixwin.h>

Xv_private void xv_init_x_pr(void);
Xv_private void xv_set_gc_op(Display *display, Xv_Drawable_info  *info, GC gc, int op, short fg_mode, int fg_bg);
Xv_private int xv_rop_internal(Display* display, Drawable d, GC gc, int x, int y, int width, int height, Xv_opaque src, int xr, int yr, Xv_Drawable_info *dest_info);
Xv_private int xv_rop_mpr_internal(Display* display, Drawable d, GC gc, int x, int y, int width, int height, Xv_opaque src, int xr, int yr, Xv_Drawable_info *dest_info, short mpr_bits);
Pkg_private void xv_to_x_convert_image(XImage *ximage, int val);
Pkg_private GC xv_find_proper_gc(Display *display, Xv_Drawable_info *info, int op);
Xv_public int xv_rop(Xv_opaque window, int x, int y, int width, int height, int op, Pixrect *pr, int xr, int yr);
Xv_private void xv_invert_polygon(Xv_opaque pw, int npts, struct pr_pos *outline, int x, int y, int w, int h, int color_index);
Xv_private void xv_gray_polygon(Xv_opaque pw, int npts, struct pr_pos *outline, int x, int y, int w, int h, Pixrect *gray_pr);
Xv_public int xv_replrop(Xv_opaque window, int xw, int yw, int width, int height, int op, Pixrect *pr, int xr, int yr);
Xv_private int xv_replrop_internal(Display *display, Xv_Drawable_info *info, Drawable d, GC gc, int xw, int yw, int width, int height, Pixrect *src, int xr, int yr, Xv_Drawable_info *dest_info);
Xv_private GC xv_get_temp_gc(Display *display, XID xid, int depth);
Xv_public Pw_pixel_cache *pw_save_pixels(register Xv_opaque pw, register Rect *rect);
Xv_public void pw_restore_pixels(register Xv_opaque pw, Pw_pixel_cache *pc);

#endif
