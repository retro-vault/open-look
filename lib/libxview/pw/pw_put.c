/*
 * Implement the pw_put functions of the pixwin.h interface.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#include <xview_private/pw_put_.h>
#include <xview_private/xv_rop_.h>

Xv_public int
pw_put(pw, x, y, val)
    Xv_opaque       pw;
    int             x, y, val;
{
    register Xv_Drawable_info *info;
    Display        *display;
    GC              gc;

    DRAWABLE_INFO_MACRO(pw, info);
    display = xv_display(info);
    gc = xv_find_proper_gc(display, info, PW_ROP_NULL_SRC);

    xv_set_gc_op(display, info, gc, PIX_SRC | PIX_COLOR(val), XV_USE_OP_FG,
		 (val) ? XV_DEFAULT_FG_BG : XV_INVERTED_FG_BG);

    XDrawPoint(display, xv_xid(info), gc, x, y);
	return 0;
}
