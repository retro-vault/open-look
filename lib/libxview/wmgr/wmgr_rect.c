/*
 * Implements the wmgr rect routines used by the XView wmgr module.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#include <xview_private/wmgr_rect_.h>
#include <xview_private/win_geom_.h>
#include <xview_private/win_global_.h>
#include <xview/win.h>

Xv_public void
wmgr_completechangerect(
		   window, rectnew, rectoriginal, parentprleft, parentprtop)
    Xv_opaque       window;
    Rect           *rectnew, *rectoriginal;
    int             parentprleft, parentprtop;
{
    (void) win_setrect(window, rectnew);
}

Xv_public void
wmgr_refreshwindow(window)
    Xv_opaque       window;
{
    Rect            rectoriginal, rectdif;
    int             marginchange = -1;

    (void) win_lockdata(window);
    (void) win_getrect(window, &rectoriginal);
    /*
     * A position change is supposed to invoke a repaint of the entire window
     * and its children. So, change position by 1,1 and then back to original
     * position.
     */
    rectdif = rectoriginal;
    if ((rectoriginal.r_width == 0) || (rectoriginal.r_height == 0))
	marginchange = 1;
    rect_marginadjust(&rectdif, marginchange);
    (void) win_setrect(window, &rectdif);
    (void) win_setrect(window, &rectoriginal);
    (void) win_unlockdata(window);
    return;
}
