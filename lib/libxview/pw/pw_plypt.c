/*
 * Implements the pw plypt routines used by the XView pw module.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#include <xview_private/pw_plypt_.h>
#include <xview_private/xv_.h>
#include <xview_private/xv_rop_.h>

Xv_public int
pw_polypoint(pw, dx, dy, npts, ptlist, op)
    register int    dx, dy, npts, op;
    register Xv_opaque pw;
    register struct pr_pos *ptlist;
{
    Xv_Drawable_info *info;
    Display        *display;
    GC              pt_gc;
    XPoint         *points;
    register int    i;

    DRAWABLE_INFO_MACRO(pw, info);
    display = xv_display(info);
    pt_gc = xv_find_proper_gc(display, info, PW_POLYPOINT);
    xv_set_gc_op(display, info, pt_gc, op, XV_USE_OP_FG,
		 XV_DEFAULT_FG_BG);

    points = (XPoint *) xv_malloc(npts * sizeof(XPoint));
    for (i = 0; i < npts; i++) {
	points[i].x = (short) ptlist[i].x + dx;
	points[i].y = (short) ptlist[i].y + dy;
    }

    XDrawPoints(display, xv_xid(info), pt_gc, points, npts, CoordModeOrigin);

    (void) free((char *) points);
	return 0;
}				/* pw_polypoint */
