/*
 * Implements the windowlayt routines used by the XView window module.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#include <xview_private/windowlayt_.h>
#include <xview_private/gettext_.h>
#include <xview_private/windowutil_.h>
#include <xview_private/win_geom_.h>
#include <xview_private/win_treeop_.h>
#include <stdio.h>
#include <xview_private/i18n_impl.h>
#include <xview/pkg.h>
#include <xview/window.h>
#include <xview/rect.h>
#include <xview/win.h>


/* ARGSUSED */
/* VARARGS3 */
Pkg_private int
window_layout(parent, child, op, d1, d2, d3, d4, d5)
    Xv_Window       parent;
    register Xv_Window child;
    Window_layout_op op;
    Xv_opaque d1, d2, d3, d4, d5;
{

    Rect            rect;
    register Window_info *child_private = WIN_PRIVATE(child);

    switch (op) {
      case WIN_CREATE:
	break;

      case WIN_INSTALL:
	if (child_private->map) {
	    win_insert(child);
	}
	break;

      case WIN_DESTROY:
	break;

      case WIN_INSERT:
	(void) win_insert(child);
	child_private->map = TRUE;
	break;

      case WIN_REMOVE:
	(void) win_remove(child);
	child_private->map = FALSE;
	break;

      case WIN_GET_BELOW:
	window_getrelrect(child, (Xv_Window) d1, &rect);
	{
	    int            *y = (int *) d2;
	    *y = rect.r_top + rect.r_height;
	    break;
	}


      case WIN_ADJUST_RECT:
	(void) win_setrect(child, (Rect *) d1);
	break;

      case WIN_GET_RIGHT_OF:
	window_getrelrect(child, (Xv_Window) d1, &rect);
	{
	    int            *x = (int *) d2;
	    *x = rect.r_left + rect.r_width;
	    break;
	}


      case WIN_GET_X:{
	    int            *x = (int *) d1;

	    (void) win_getrect(child, &rect);
	    *x = rect.r_left;
	    break;
	}

      case WIN_GET_Y:{
	    int            *y = (int *) d1;

	    (void) win_getrect(child, &rect);
	    *y = rect.r_top;
	    break;
	}

      case WIN_GET_WIDTH:{
	    int            *w = (int *) d1;

	    (void) win_getrect(child, &rect);
	    *w = rect.r_width;
	    break;
	}

      case WIN_GET_HEIGHT:{
	    int            *h = (int *) d1;

	    (void) win_getrect(child, &rect);
	    *h = rect.r_height;
	    break;
	}

      case WIN_GET_RECT:{
	    Rect           *r = (Rect *) d1;
	    (void) win_getrect(child, r);
	    break;
	}

      default:{
	    char            dummy[128];

	    (void) sprintf(dummy, 
		XV_MSG("window layout option (%d) not recognized (window_layout)"),
			   op);
	    xv_error((Xv_object)NULL,
		     ERROR_STRING, dummy,
		     ERROR_PKG, WINDOW,
		     NULL);
	    return FALSE;
	}
    }
    return TRUE;
}
