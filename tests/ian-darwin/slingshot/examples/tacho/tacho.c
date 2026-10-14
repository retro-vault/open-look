/*
 *	(c) Copyright 1991 Sun Microsystems, Inc.  All rights reserved.
 *	See LEGAL_NOTICE file for terms of the license.
 *
 *	@(#) tacho.c 1.7 92/10/22 
 */

#include <xview/frame.h>
#include <xview/panel.h>
#include <xview/cms.h>
#include <sspkg/canshell.h> 
#include <sspkg/drawobj.h> 
#include <sspkg/box.h> 
 
Frame		frame;
Canvas_shell	shell;
Cbox		cbox;
Tacho		tacho;

#define ATTR_KEY XV_KEY_DATA, attr_key
int attr_key;

int main(int argc, char	* argv[])
{
	Panel		panel;
	Panel_item	ptext;
	Cms		cms;
	void		set_opaque();
	Panel_item	create_attr_item();
	void		resize();

	xv_init(XV_INIT_ARGC_PTR_ARGV, &argc, argv, 0);
	attr_key = xv_unique_key();
 
	cms = (Cms) xv_create(XV_NULL, CMS,
		CMS_CONTROL_CMS, TRUE,
		CMS_SIZE, CMS_CONTROL_COLORS + 2,
		CMS_NAMED_COLORS, "white", "black", 0,
		0);

	frame = (Frame) xv_create(XV_NULL, FRAME,
			FRAME_LABEL, argv[0],
			0);
 
	panel = (Panel) xv_create(frame, PANEL,
			PANEL_LAYOUT, PANEL_VERTICAL,
			0);

	(void) create_attr_item(panel, "TACHO_VALUE",
		TACHO_VALUE, 75);
	(void) create_attr_item(panel, "XV_WIDTH", 
		XV_WIDTH, 100);
	(void) create_attr_item(panel, "XV_HEIGHT", 
		XV_HEIGHT, 100);
	(void) create_attr_item(panel, "TACHO_MIN_VALUE", 
		TACHO_MIN_VALUE, 0);
	(void) create_attr_item(panel, "TACHO_MAX_VALUE", 
		TACHO_MAX_VALUE, 100);

	window_fit(panel);
 
	shell = (Canvas_shell) xv_create(frame, CANVAS_SHELL, 
			WIN_CMS, cms,
			XV_X, 0,
			XV_Y, 0,
			CANVAS_MIN_PAINT_WIDTH, 100,
			CANVAS_MIN_PAINT_HEIGHT, 110,
			XV_HEIGHT, 110,
			WIN_BELOW, panel,
			0);

	cbox = (Cbox) xv_create(shell, CBOX,
			0);

	tacho = (Tacho) xv_create(cbox, TACHO,
			XV_X, 10,
			XV_Y, 10,
			XV_WIDTH, 100,
			XV_HEIGHT, 100,
			TACHO_MIN_VALUE, 0,
			TACHO_MAX_VALUE, 100,
			TACHO_VALUE, 75,
			RECTOBJ_BG2, CMS_CONTROL_COLORS,/*white*/
			0);

	xv_set(shell,
		CANVAS_RESIZE_PROC, resize,
		0);

	window_fit(shell);
	window_fit(frame);
	xv_set(panel, 
		XV_WIDTH, WIN_EXTEND_TO_EDGE, 
		0);

	xv_main_loop(frame); 
} 
 
void
resize(Canvas_shell canvas_shell, int w, int h)
{
	xv_set(cbox,
		XV_WIDTH, w,
		XV_HEIGHT, h,
		0);
}

void
set_attr(Panel_item item, int value, Event		* event)
{
	int attr;

	attr = xv_get(item, ATTR_KEY);
	xv_set(tacho,
		attr, value,
		0);
}

Panel_item
create_attr_item(Panel panel, char * label, int attr, int value)
{
	return (Panel_item) xv_create(panel, PANEL_SLIDER,
			PANEL_LABEL_STRING, label,
			PANEL_VALUE, value,
			PANEL_NOTIFY_PROC, set_attr,
			PANEL_NOTIFY_LEVEL, PANEL_ALL,
			ATTR_KEY, attr,
			0);
}


