/*
 * Implements the sb destroy routines used by the XView scrollbar
 * module.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#include <xview_private/sb_destroy_.h>
#include <xview/canvas.h>
#include <xview/frame.h>
#include <xview/screen.h>

/******************************************************************/

Pkg_private int
scrollbar_destroy_internal(sb_public, status)
    Scrollbar       sb_public;
    Destroy_status  status;
{
    Xv_scrollbar_info *sb = SCROLLBAR_PRIVATE(sb_public);
    Xv_Window	    focus_win;
    Frame	    frame;
	
    if ((status == DESTROY_CLEANUP) || (status == DESTROY_PROCESS_DEATH)) {
	/* If the scrollbar owns the Frame focus window, then ???
	 */
	frame = xv_get(sb_public, WIN_FRAME);
	focus_win = xv_get(frame, FRAME_FOCUS_WIN);
	if (focus_win && xv_get(focus_win, WIN_PARENT) == sb_public) {
	    xv_set(focus_win,
		   WIN_PARENT, frame,  /* the only window guaranteed still
					* to exist. */
		   XV_SHOW, FALSE,
		   0);
	    /* BUG ALERT:  If the canvas is the only frame subwindow,
	     *		   we will be left without a current focus subwindow.
	     */
	    xv_set(frame, FRAME_NEXT_PANE, 0);
	}
	
	/* Clean up menu */
	xv_destroy(sb->menu);
	
	if (status == DESTROY_CLEANUP)
	    free((char *) sb);
    }
    return XV_OK;
}


	
