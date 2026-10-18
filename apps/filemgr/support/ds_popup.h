/*	@(#)ds_popup.h 3.2 IEI SMI	*/

/*
 * Copyright (c) 1990 by Sun Microsystems, Inc.
 */

#ifndef	_ds_popup_h
#define	_ds_popup_h

/*
 * Location ops for ds_position_popup()
 */
enum ds_location_op {
	DS_POPUP_RIGHT,		/* Place popup to right of baseframe */
	DS_POPUP_LEFT,		/* Place popup to left of baseframe */
	DS_POPUP_ABOVE,		/* Place popup above baseframe */
	DS_POPUP_BELOW,		/* Place popup below baseframe */
	DS_POPUP_LOR,		/* Place popup to right or left of baseframe */
	DS_POPUP_AOB,		/* Place popup above or below baseframe */
	DS_POPUP_CENTERED	/* Center popup within baseframe */
};

int	ds_position_popup();
int	ds_position_popup_rect();
int	ds_force_popup_on_screen();
int	ds_get_screen_size();
int	ds_center_items(Panel panel, int row, ...);

#endif /*!_ds_popup_h*/
