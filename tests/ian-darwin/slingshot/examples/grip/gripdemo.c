/*
 *	(c) Copyright 1991 Sun Microsystems, Inc.  All rights reserved.
 *	See LEGAL_NOTICE file for terms of the license.
 *
 *	@(#) gripdemo.c 1.2 92/06/24 
 */

#include <X11/Xlib.h>
#include <xview/xview.h>
#include <xview/rect.h>
#include <xview/win_input.h>
#include <sspkg/canshell.h>
#include <sspkg/rectobj.h>
#include <sspkg/box.h>
#include <sspkg/drawobj.h>
#include <sspkg/grip.h>


typedef struct {
	char			*label;
	Grip_rubber_style	style;
} My_grip_list;


int main(int argc, char           * argv[])
{
	Frame		frame;
	Canvas_shell	canvas_shell;
	Box		box1;
	Box		box2;
	Drawrect	drawrect1;
	Drawrect	drawrect2;
	int		i;

	static My_grip_list grips[] = {
		"GRIP_RUBBER_NONE", 		GRIP_RUBBER_NONE,
		"GRIP_RUBBER_RECT",	 	GRIP_RUBBER_RECT,
		"GRIP_RUBBER_VLINE", 		GRIP_RUBBER_VLINE,
		"GRIP_RUBBER_VLINE_PAIR",	GRIP_RUBBER_VLINE_PAIR,
		"GRIP_RUBBER_HLINE", 		GRIP_RUBBER_HLINE,
		"GRIP_RUBBER_HLINE_PAIR",	GRIP_RUBBER_HLINE_PAIR,
		"GRIP_RUBBER_CROSSHAIRS",	GRIP_RUBBER_CROSSHAIRS,
	};

	xv_init(XV_INIT_ARGC_PTR_ARGV, &argc, argv, 0);

	frame = (Frame) xv_create(XV_NULL, FRAME, 0);

	canvas_shell = xv_create(frame, CANVAS_SHELL,
		XV_HEIGHT, 600,
		0);

	box1 = xv_create(canvas_shell, BOX,
		BOX_LAYOUT, BOX_LAYOUT_VERTICAL,
		BOX_GAP, 15,
		RECTOBJ_BORDER, 10,
		0);

	for(i=0;i<sizeof(grips)/sizeof(My_grip_list);i++) {
		box2 = xv_create(box1, BOX,
			BOX_LAYOUT, BOX_LAYOUT_VERTICAL,
			0);
		(void) xv_create(box2, DRAWTEXT,
			XV_WIDTH, 150,
			DRAWTEXT_STRING, grips[i].label,
			0);
		drawrect1 =  xv_create(box2, DRAWRECT,
			XV_WIDTH, 400,
			XV_HEIGHT, 75,
			RECTOBJ_EVENT_PROC, 0,
			0);
		(void) xv_create(drawrect1, GRIP,
			XV_X, xv_get(drawrect1, XV_WIDTH)/2,
			XV_Y, xv_get(drawrect1, XV_HEIGHT)/2,
			GRIP_RUBBER_STYLE, grips[i].style,
			0);
	}

	xv_set(canvas_shell,
		XV_WIDTH, xv_get(box1, XV_WIDTH),
		XV_HEIGHT, xv_get(box1, XV_HEIGHT),
		0);

	window_fit(frame);
	xv_main_loop(frame);
}


#ifdef EXAMPLE_CALLBACKS

void 
grip_double_clk_proc(Xv_window paint_window, Event		* event, Canvas_shell canvas_shell, Grip grip)
{
	/* RECTOBJ_DBL_CLICK_PROC */
	printf("dbl click on %d\n", grip);
}

void 
grip_done_drag_proc(Xv_window paint_window, Event		* event, Canvas_shell canvas_shell, Grip grip, int x, int y)
{
	/* GRIP_DONE_PROC */
	printf("started dragging at (%d, %d)\n", 
		xv_get(grip, GRIP_BTN_DOWN_X),
		xv_get(grip, GRIP_BTN_DOWN_Y));
	printf("ended dragging at (%d, %d)\n", x, y);
}


int
grip_move_proc(Xv_window paint_window, Event		* event, Canvas_shell canvas_shell, Grip grip, short * new_x, short * new_y)
{
	/* GRIP_MOVE_PROC */
	printf("moving grip to %d, %d\n", *new_x, *new_y);
	return(TRUE);
}

#endif 

