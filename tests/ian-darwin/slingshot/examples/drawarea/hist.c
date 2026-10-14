#include <xview/xview.h>
#include <sspkg/drawobj.h>
#include <sspkg/disp_list.h>
/*
 *	(c) Copyright 1991 Sun Microsystems, Inc.  All rights reserved.
 *	See LEGAL_NOTICE file for terms of the license.
 *
 *	@(#) hist.c 1.3 92/06/26 
 */

#include <xview/frame.h>
#include <xview/panel.h>
#include <sspkg/canshell.h> 
#include <sspkg/rectobj.h> 
#include <sspkg/drawobj.h> 
 
void		resize();
void		set_data();

#define NUM_DATA	10
Drawarea	hist;
Panel_item	slider[NUM_DATA];
double		data[NUM_DATA];
int		key;

int main(int argc, char	* argv[])
{
	Frame		frame;
	Panel		panel;
	Canvas_shell	shell;
	int		i;
	double left =	0.0;
	double right=	NUM_DATA;
	double upper = 1.0;
	double lower = 0.0;

	xv_init(XV_INIT_ARGC_PTR_ARGV, &argc, argv, 0);

	frame = (Frame) xv_create(XV_NULL, FRAME, 
		FRAME_LABEL, argv[0],
		XV_WIDTH, 800,
		XV_HEIGHT, 400,
		0);
 
	panel = (Panel) xv_create(frame, PANEL,
		PANEL_LAYOUT, PANEL_VERTICAL,
		0);

	key = xv_unique_key();
	for(i=0; i<NUM_DATA; i++) {
		slider[i] = (Panel_item) xv_create(panel, PANEL_SLIDER, 
			XV_KEY_DATA, key, i,
			PANEL_NOTIFY_PROC, set_data,
			0);
	}

	window_fit(panel);

	shell = (Canvas_shell) xv_create(frame, CANVAS_SHELL, 
		WIN_BELOW, panel,
		XV_X, 0,
		XV_WIDTH, xv_get(panel, XV_WIDTH),
		XV_HEIGHT, 200,
		0);

	hist = (Drawarea) xv_create(shell, DRAWAREA,
		RECTOBJ_RESIZABLE, TRUE,
		DRAWAREA_LEFT_X, &left,
		DRAWAREA_RIGHT_X, &right,
		DRAWAREA_UPPER_Y, &upper,
		DRAWAREA_LOWER_Y, &lower,
		XV_WIDTH, xv_get(shell, XV_WIDTH),
		XV_HEIGHT, xv_get(shell, XV_HEIGHT),
		0);

	xv_set(shell,
		CANVAS_RESIZE_PROC, resize,
		0);

	window_fit(frame);
	xv_main_loop(frame); 
} 
 

void
resize(Canvas_shell shell, int width, int height)
{
	xv_set(hist,
		XV_WIDTH, width,
		XV_HEIGHT, height,
		0);
}

int display_changes(void)
{
	int i;
	double max = 0.0;

	VClear(hist);
	for(i=0; i<NUM_DATA; i++) {
		DFillRectangle(hist, 
			.05 + i, 0.0,
			.95, data[i]);

		if(data[i] > max)
			max = data[i];
	}
	xv_set(hist,
		DRAWAREA_UPPER_Y, &max,
		0);
}


void
set_data(Panel_item item, int value, Event		* event)
{
	int i;
	
	i = (int) xv_get(item, XV_KEY_DATA, key);
	data[i]  = value;
	display_changes();
}
