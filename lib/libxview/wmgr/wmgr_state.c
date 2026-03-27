/*
 * Implements the wmgr state routines used by the XView wmgr module.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#include <xview_private/wmgr_state_.h>
#include <xview_private/win_treeop_.h>
#include <stdio.h>
#include <xview/win_struct.h>
#include <xview/win.h>

Xv_public void
wmgr_changelevel(window, parent, top)
    register Xv_object window;
    int             parent;
    int             top;
{
    int             topchildnumber, bottomchildnumber;

    /*
     * Don't try to optimize by not doing anything if already at desired
     * level.  Doing so messes up the fixup list because callers of this
     * routine do partial repair which incorrectly removes some stuff from
     * the damage list if a win_remove/win_insert pair hasn't been done.
     */
    /*
     * Remove from tree
     */
    (void) win_remove(window);
    /*
     * Set new links
     */
    if (top) {
	topchildnumber = win_getlink(parent, WL_TOPCHILD);
	(void) win_setlink(window, WL_COVERED, topchildnumber);
	(void) win_setlink(window, WL_COVERING, WIN_NULLLINK);
    } else {
	bottomchildnumber = win_getlink(parent, WL_BOTTOMCHILD);
	(void) win_setlink(window, WL_COVERING, bottomchildnumber);
	(void) win_setlink(window, WL_COVERED, WIN_NULLLINK);
    }
    /*
     * Insert into tree
     */
    (void) win_insert(window);
    return;
}
