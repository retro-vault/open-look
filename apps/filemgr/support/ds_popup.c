#ifndef lint
static 	char sccsid[] = "@(#)ds_popup.c 3.3 92/10/23 Copyr 1990 Sun Micro";
#endif

/*
 * Copyright (c) 1990 by Sun Microsystems, Inc.
 */

#ifdef __STDC__
#include <stdarg.h>
#else /* __STDC__ */
#include <sys/varargs.h>
#endif /* __STDC__ */
#include <xview/xview.h>
#include <xview/panel.h>
#include "ds_popup.h"

/******************************************************************************
*
*       Function:	ds_position_popup
*
*       Description:	Position a popup relative to the parent frame
*			making sure it doesn't go off of the screen.
*
*       Parameters:     base		Popup's parent frame
*			popup		Popup frame
*			location_op	Where you would like the popup to
*					appear.  Location_op may be any
*					the following:
*
*	DS_POPUP_LEFT	Place the popup to the left of base with the tops flush
*	DS_POPUP_RIGHT	Place the popup to the right of base with the tops flush
*	DS_POPUP_ABOVE	Place the popup above base with the left edges flush
*	DS_POPUP_BELOW	Place the popup below base with the left edges flush
*	DS_POPUP_LOR	Place the popup either to the left or right of base
*			depending on which side has the most space.
*	DS_POPUP_AOF	Place the popup either above or below base
*			depending on which side has the most space.
*	DS_POPUP_CENTERED	Center popup within baseframe
*
*       Returns:        0	Could not get screen size
*			1	All is well
*
******************************************************************************/
int
ds_position_popup(base, popup, location_op)

	Frame	base;
	Frame	popup;
	enum ds_location_op	location_op;
{
	Rect	base_rect;

	frame_get_rect(base, &base_rect);

	return (ds_position_popup_rect(&base_rect, popup, location_op));
}

/******************************************************************************
*
*       Function:	ds_position_popup_rect
*
*       Description:	Position a popup relative to the parent frame
*			making sure it doesn't go off of the screen.
*
*			We added this rect interface to support the "move"
*			tooltalk message, since the app will not have
*			the baseframe, just its rect.
*
*
*       Parameters:     base_rect_p	Pointer to parent's frame rect
*			popup		popup to position
*			location_op	Where you would like the popup to
*					appear.  Location_op may be any
*					the following:
*
*	DS_POPUP_LEFT	Place the popup to the left of base with the tops flush
*	DS_POPUP_RIGHT	Place the popup to the right of base with the tops flush
*	DS_POPUP_ABOVE	Place the popup above base with the left edges flush
*	DS_POPUP_BELOW	Place the popup below base with the left edges flush
*	DS_POPUP_LOR	Place the popup either to the left or right of base
*			depending on which side has the most space.
*	DS_POPUP_AOF	Place the popup either above or below base
*			depending on which side has the most space.
*	DS_POPUP_CENTERED	Center popup within baseframe
*
*       Returns:        0	Could not get screen size
*			1	All is well
*
******************************************************************************/
int
ds_position_popup_rect(base_rect_p, popup, location_op)

	Rect	*base_rect_p;
	Frame	popup;
	enum ds_location_op	location_op;

{
	Rect	popup_rect;
	int	screen_width, screen_height;
	int	base_x, base_y, popup_x, popup_y, base_width, base_height,
		popup_width, popup_height;


	frame_get_rect(popup, &popup_rect);

	ds_get_screen_size(popup, &screen_width, &screen_height);

	base_x = base_rect_p->r_left;
	base_y = base_rect_p->r_top;
	base_width = base_rect_p->r_width;
	base_height = base_rect_p->r_height;
	popup_width = popup_rect.r_width;
	popup_height = popup_rect.r_height;

	if (location_op == DS_POPUP_LOR) {
		if (base_x >= screen_width - base_width - base_x)
			location_op = DS_POPUP_LEFT;
		else
			location_op = DS_POPUP_RIGHT;
	} else if (location_op == DS_POPUP_AOB) {
		if (base_y > screen_height - base_height - base_y)
			location_op = DS_POPUP_ABOVE;
		else
			location_op = DS_POPUP_BELOW;
	}

	switch (location_op) {

	case DS_POPUP_RIGHT:
		popup_x = base_x + base_width;
		popup_y = base_y;
		break;
	case DS_POPUP_LEFT:
		popup_x = base_x - popup_width;
		popup_y = base_y;
		break;
	case DS_POPUP_ABOVE:
		popup_x = base_x;
		popup_y = base_y - popup_height;
		break;
	case DS_POPUP_BELOW:
		popup_x = base_x;
		popup_y = base_y + base_height;
		break;
	case DS_POPUP_CENTERED:
	default:
		popup_x = base_x + (base_width - popup_width) / 2;
		popup_y = base_y + (base_height - popup_height) / 2;
		break;
	}
	ds_force_popup_on_screen(&popup_x, &popup_y, NULL, popup,
				screen_width, screen_height);
}

/******************************************************************************
*
*       Function:	ds_force_popup_on_screen
*
*       Description:	Make sure that the specified frame appears entirely
*			on the screen.
*
* 			You specify the x and y where you would like the
*			popup to appear.  If this location would cause any
*			portion of the popup to appear off of the screen
*			then the routine makes the minimum adjustments
*			necessary to move it onto the screen.
*
*			NOTE:	The following coordinates must be specified
*				relative to the screen origin *not* the
*				parent frame! (as in frame_rects as
*				opposed to XV_X, XV_Y);
*
*       Parameters:     popup_x_p	Pointer to x location where you would
*					like the popup to appear.  If the popup
*					is moved this is updated to reflect
*					the new position.
*       		popup_y_p	Pointer to y location where you would
*					like the popup to appear.  If the popup
*					is moved this is updated to reflect
*					the new position.
*					popup to appear
*			base		Popup's parent frame
*					No longer used.  Left for
*					backwards compatibility.
*			popup		Popup`s frame
*
*       Returns:        TRUE	The popup was moved
*			FALSE	The popup was not moved
*
******************************************************************************/
int
ds_force_popup_on_screen(popup_x_p, popup_y_p, base, popup)

	int	*popup_x_p, *popup_y_p;
	Frame	base, popup;

{
	Rect	popup_rect;
	int	popup_x, popup_y;
	int	screen_width, screen_height;
	int	popup_width, popup_height;
	int	n;
	int	rcode;

	popup_x = *popup_x_p;
	popup_y = *popup_y_p;

	/* Get the screen size */
	ds_get_screen_size(popup, &screen_width, &screen_height);

	frame_get_rect(popup, &popup_rect);
	popup_width = popup_rect.r_width;
	popup_height = popup_rect.r_height;

	/* Make sure frame does not go off side of screen */
	n = popup_x + popup_width;
	if (n > screen_width)
		popup_x -= (n - screen_width);
	else if (popup_x < 0)
		popup_x = 0;

	/* Make sure frame doen't go off top or bottom */
	n = popup_y + popup_height;
	if (n > screen_height)
		popup_y -= n - screen_height;
	else if (popup_y < 0)
		popup_y = 0;

	/* Set location and return */
	popup_rect.r_left = popup_x;
	popup_rect.r_top = popup_y;
	frame_set_rect(popup, &popup_rect);

	if (popup_x != *popup_x_p || popup_y != *popup_y_p)
		rcode = TRUE;
	else
		rcode = FALSE;
	*popup_x_p = popup_x;
	*popup_y_p = popup_y;
	return(rcode);
}

/******************************************************************************
*
*       Function:	ds_get_screen_size
*
*       Description:	Get the width and height of the screen in pixels
*
*       Parameters:     width_p		Pointer to an integer to place width
*			height_p	Pointer to an integer to place height
*
*       Returns:        1	All is well
*
******************************************************************************/
int
ds_get_screen_size(frame, width_p, height_p)

Frame	frame;
int	*width_p;
int	*height_p;

{
	Rect	*scr_size;

	scr_size = (Rect *)xv_get(frame, WIN_SCREEN_RECT);
	*width_p = scr_size->r_width;
	*height_p = scr_size->r_height;

	return(1);
}

/******************************************************************************
*
*       Function:	ds_center_items
*
*       Description:	Center items on a given row
*
*       Parameters:     panel		Panel containing items
*			row		Row to place items on. -1 to use y
*					location of first item.
*			Null terminated list of items to center.
*
*	ie:	ds_center_items(panel, 2, button1, button2, button3, 0);
*
* 	Note that the panel must have its width set before calling this
*	function.
*
*       Returns:        x location of leftmost item
*
******************************************************************************/
/* VARARGS1 */
int
#ifdef __STDC__
ds_center_items(Panel panel, int row, ...)
#else /* __STDC__ */
ds_center_items(panel, row, va_alist)

	Panel	panel;
	int	row;
	va_dcl
#endif /* __STDC__ */
{
	va_list	args;
	register int	y;		/* y location */
	register int	x;		/* x location */
	register int	width;		/* Total width of items + gaps */
	int	x_gap;			/* Gap between items */
	Rect *	rect;			/* Item rects */
	int	rcode;
	Panel_item	item;

	x_gap = (int)xv_get(panel, PANEL_ITEM_X_GAP);

	if (row < 0)
		y = -1;
	else
		y = xv_row(panel, row);

	/*
	 * Sum up total width of all items and gaps
	 */
	width = 0;

#ifdef __STDC__
	va_start(args, row);
#else /* __STDC__ */
	va_start(args);
#endif /* __STDC__ */

	while ((item = (Panel_item )va_arg(args, Panel_item *)) != 0) {
		if (y < 0)
			y = (int)xv_get(item, XV_Y);
		rect = (Rect *)xv_get(item, PANEL_ITEM_RECT);
		width += rect->r_width + x_gap;
	}
	va_end(args);

	width -= x_gap;

	/*
	 * Set x location of leftmost item
	 */
	x = ((int)xv_get(panel, XV_WIDTH) - width) / 2;

	if (x < 0)
		x = 0;

	rcode = x;

	/*
	 * Position items
	 */
#ifdef __STDC__
	va_start(args, row);
#else /* __STDC__ */
	va_start(args);
#endif /* __STDC__ */

	while ((item = (Panel_item )va_arg(args, Panel_item *)) != 0) {
		xv_set(item, XV_X, x, XV_Y, y, 0);
		rect = (Rect *)xv_get(item, PANEL_ITEM_RECT);
		x += rect->r_width + x_gap;
	}
	va_end(args);

	return(rcode);
}
