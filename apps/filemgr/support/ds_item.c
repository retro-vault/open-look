#ifndef lint
static 	char sccsid[] = "@(#)ds_item.c 3.3 92/09/08 Copyr 1990 Sun Micro";
#endif

/*
 * Copyright (c) 1990 by Sun Microsystems, Inc.
 */

#include <xview/xview.h>
#include <xview/panel.h>
#include <xview/font.h>
#include "ds_item.h"

/******************************************************************************
*
*       Function:       ds_justify_items
*
*       Description:    Right justify the labels of all panel items in a
*			panel.  This is usefull for lining up items in
*			property sheets and popups.  Note, all panel items
*			should have their Y-positions set prior to this
*			function.
*
*       Parameters:     panel	Panel containing items
*                       resize	TRUE to resize text fields so that they
*				extend to the right edge of the panel.
*				For this option to work, the width of the
*				panel must already be set to its desired value.
*
*       Returns:        PANEL_VALUE_X of the fields.
*
******************************************************************************/
int
ds_justify_items(panel, resize)

	Panel	panel;
	int	resize;		/* TRUE to resize text fields */

{
	register Panel_item	item;
	register int		value_x;
	register Panel_item_type class;
	Xv_Font			font = 0;
	Font_string_dims	font_size;
	int			longest = 0;
	char			*string;

	if (panel == 0)
		return(-1);

	/*
	 * Compute the longest label excluding all panel buttons
	 */
	PANEL_EACH_ITEM(panel, item) {
		if ((int)xv_get(item, PANEL_SHOW_ITEM) && ((Panel_item_type)
		    xv_get(item, PANEL_ITEM_CLASS) != PANEL_BUTTON_ITEM)) {
			font = (Xv_Font)xv_get(item, PANEL_LABEL_FONT);
			string = (char *)xv_get(item, PANEL_LABEL_STRING);
			(void)xv_get(font,FONT_STRING_DIMS, string, &font_size);
			if (font_size.width > longest)  {
				longest = font_size.width;
			}
		}
	} PANEL_END_EACH;

	value_x = longest + 2 * (int)xv_get(panel, PANEL_ITEM_X_GAP);

	/* Layout each item (except buttons) on the panel */
	PANEL_EACH_ITEM(panel, item) {
		if ((int)xv_get(item, PANEL_SHOW_ITEM) &&
		    ((class = (Panel_item_type)xv_get(item, PANEL_ITEM_CLASS))
		    		!= PANEL_BUTTON_ITEM)) {
			(void)xv_set(item, PANEL_VALUE_X, value_x, 0);
			if (resize && class == PANEL_TEXT_ITEM) {
				(void)ds_resize_text_item(panel, item);
			}
		}
	}
	PANEL_END_EACH;

	return(value_x);
}
/******************************************************************************
*
*       Function:       ds_resize_left_text_item
*
*       Description:    Set the width of a text item so that it extends
*			from its current location to the supplied location
*			x.
*
*       Parameters:     panel		Panel containing text fields
*                       text_item	Text item to resize
*			loc_x		resize up to loc_x
*
*       Returns:        PANEL_VALUE_DISPLAY_LENGTH of the text item
*
******************************************************************************/
int
ds_resize_left_text_item(panel, text_item, loc_x)
	Panel		panel;
	Panel_item	text_item;
	int		loc_x;

{
	Xv_Font	font;
	int	width;
	int	n;

	if (panel == 0 || text_item == 0)
		return(-1);

	/*
	 * Set the display width of the fillin field to extend to the
	 * loc_x.
	 */
	width = loc_x -
		(int)xv_get(text_item, PANEL_VALUE_X) -
		(int)xv_get(panel, PANEL_ITEM_X_GAP);

	font = (Xv_Font)xv_get(panel, XV_FONT);
#ifdef OW_I18N
	n = width / (int)xv_get(font, FONT_DEFAULT_CHAR_WIDTH);
#else
	n = width / (int)xv_get(font, FONT_DEFAULT_CHAR_WIDTH);
#endif /*
	 * Make sure it gets no smaller than 5 characters and no larger
	 * than the stored length.
	 */
	if (n < 5)
		n = 5;
	else if (n > (int)xv_get(text_item, PANEL_VALUE_STORED_LENGTH))
		n = (int)xv_get(text_item, PANEL_VALUE_STORED_LENGTH);

	(void)xv_set(text_item, PANEL_VALUE_DISPLAY_LENGTH, n, 0);

	return(n);
}

/******************************************************************************
*
*       Function:       ds_resize_text_item
*
*       Description:    Set the width of a text item so that it extends
*			from its current location to the right edge of the
*			panel.
*
*       Parameters:     panel		Panel containing text fields
*                       text_item	Text item to resize
*
*       Returns:        PANEL_VALUE_DISPLAY_LENGTH of the text item
*
******************************************************************************/
int
ds_resize_text_item(panel, text_item)

	Panel		panel;
	Panel_item	text_item;

{
	if (panel == 0 || text_item == 0)
		return(-1);

	/*
	 * Set the display width of the fillin field to extend to the
	 * right edge of the panel.
	 */
        return(ds_resize_left_text_item(panel,
			  text_item, (int)xv_get(panel, XV_WIDTH)));
}

/******************************************************************************
*
*       Function:       ds_clear_text_items
*
*       Description:    Clears all visible text items in a panel
*
*       Parameters:     panel	Panel containing text fields
*
*       Returns:        void
*
******************************************************************************/
void
ds_clear_text_items(panel)

	Panel	panel;

{
	register Panel_item	item;

	if (panel == 0)
		return;

	PANEL_EACH_ITEM(panel, item) {
		if (xv_get(item, PANEL_SHOW_ITEM) &&
			((Panel_item_type) xv_get(item, PANEL_ITEM_CLASS) ==
							PANEL_TEXT_ITEM)) {

			(void)xv_set(item, PANEL_VALUE, "", 0);
		}
	} PANEL_END_EACH;

	return;
}
