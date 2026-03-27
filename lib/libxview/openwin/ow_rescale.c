/*
 * Implements the ow rescale routines used by the XView openwin module.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#include <xview_private/ow_rescale_.h>
#include <xview_private/ow_resize_.h>
#include <xview_private/ow_view_.h>
#include <xview_private/windowutil_.h>
#include <xview/font.h>
#include <xview_private/windowimpl.h>

/*-------------------Function Definitions-------------------*/

/*
 * openwin_rescale - resize the openwin for the given scale
 */
Pkg_private void
openwin_rescale(owin_public, scale)
    Openwin         owin_public;
    int             scale;
{
    Xv_openwin_info *owin = OPENWIN_PRIVATE(owin_public);
    Openwin_view_info *view = owin->views;
    Window_rescale_rect_obj *rect_obj_list;
    int             num_views = 0, i = 0;
    int             parent_width, parent_height;
    Rect            new_rect, parent_new_rect;

    /*
     * first change scale unless this has been in the event func
     */

    parent_new_rect = *(Rect *) xv_get(owin_public, WIN_RECT);
    parent_width = parent_new_rect.r_width;
    parent_height = parent_new_rect.r_height;

    /*
     * Openwin rect has been set. The rescale has changed the font as well
     */

    num_views = openwin_count_views(owin);
    rect_obj_list = window_create_rect_obj_list(num_views);

    for (view = owin->views; view != NULL; view = view->next_view) {
	window_set_rescale_state(view->view, scale);
	window_start_rescaling(view->view);
	/* third arg has to be address [vmh - 10/16/90] */
	window_add_to_rect_list(rect_obj_list, view->view, &view->enclosing_rect, i);
	i++;
    }
    /* FIXME: arguments are messed up and I don't understand this */
    /* window_adjust_rects(rect_obj_list, num_views, parent_width, parent_height);*/
    window_adjust_rects(rect_obj_list, (Xv_Window)owin_public, num_views, parent_width, parent_height);
    i = 0;
    for (view = owin->views; view != NULL; view = view->next_view) {
	if (!window_rect_equal_ith_obj(rect_obj_list, &new_rect, i))
	    openwin_adjust_view(owin, view, &new_rect);
	window_end_rescaling(view->view);
    }
    window_destroy_rect_obj_list(rect_obj_list);
}
