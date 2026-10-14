#include <xview/xview.h>
#include <sspkg/drawobj.h>
#include <sspkg/disp_list.h>
/*
 *	(c) Copyright 1991 Sun Microsystems, Inc.  All rights reserved.
 *	See LEGAL_NOTICE file for terms of the license.
 *
 * 	@(#) temp_grip.c 1.4 92/11/12 
 */
#include <xview/xview.h>
#include <sspkg/canshell.h>
#include <sspkg/rectobj.h>
#include <sspkg/drawobj.h>
#include <sspkg/box.h>
#include <sspkg/grip.h>

static unsigned short smiley_bits[] = {
#include "../icons/smiley.icon"
};

static unsigned short frown_bits[] = {
#include "../icons/frown.icon"
};

static unsigned short amper_bits[] = {
#include "../icons/amper.icon"
};

void highlight_clockobj();
void start_drag_proc();


int main(int argc, char           * argv[])
{
	Frame		frame;
	Canvas_shell	shell;
	Bag		bag;
	Server_image    ampersand;
	Server_image    smiley, frown;
	Clockobj	c;
	

	xv_init(XV_INIT_ARGC_PTR_ARGV, &argc, argv, 0);

	frame = (Frame) xv_create(XV_NULL, FRAME,
			FRAME_LABEL, argv[0],
			0);

	smiley = (Server_image) xv_create(XV_NULL, SERVER_IMAGE,
			XV_WIDTH, 32,
			XV_HEIGHT, 20,
			SERVER_IMAGE_BITS, smiley_bits,
			0);

	frown = (Server_image) xv_create(XV_NULL, SERVER_IMAGE,
			XV_WIDTH, 32,
			XV_HEIGHT, 20,
			SERVER_IMAGE_BITS, frown_bits,
			0);

	ampersand = (Server_image) xv_create(XV_NULL, SERVER_IMAGE,
			XV_WIDTH, 64,
			XV_HEIGHT, 49,
			SERVER_IMAGE_BITS, amper_bits,
			0);

	shell = (Canvas_shell) xv_create(frame, CANVAS_SHELL,
			0);
	
	bag = (Bag) xv_create(shell, BAG,
			BAG_AUTO_SHRINK, FALSE,
			BAG_ANCHORED, TRUE,
			0);
	/* 
	 * Create some objects inside the bag so the user can drag them.
	 */

	(void) xv_create(bag, DRAWTEXT,
		XV_X, 10,
		XV_Y, 10,
		XV_HEIGHT, 30,
		XV_WIDTH, 50,
		DRAWTEXT_JUSTIFY, DRAWTEXT_JUSTIFY_CENTER,
		DRAWTEXT_STRING, "DragMe",
		RECTOBJ_START_DRAG_PROC, start_drag_proc,
		0);

	(void) xv_create(bag, DRAWIMAGE,
		XV_X, 40,
		XV_Y, 40,
		DRAWIMAGE_SVRIMAGE, ampersand,
		RECTOBJ_START_DRAG_PROC, start_drag_proc,
		0);

	(void) xv_create(bag, DRAWICON,
		XV_X, 80,
		XV_Y, 80,
		DRAWIMAGE_SVRIMAGE, smiley,
		DRAWIMAGE_HIGHLIGHT_IMAGE, frown,
		DRAWTEXT_STRING, "Drag Me",
		DRAWTEXT_EDITABLE, TRUE,
		DRAWTEXT_LENGTH, 8,
		DRAWTEXT_JUSTIFY, DRAWTEXT_JUSTIFY_CENTER,
		RECTOBJ_START_DRAG_PROC, start_drag_proc,
		0);

	(void) xv_create(bag, DRAWICON,
		XV_X, 120,
		XV_Y, 120,
		DRAWIMAGE_SVRIMAGE, smiley,
		DRAWIMAGE_HIGHLIGHT_IMAGE, frown,
		DRAWTEXT_STRING, "Drag Me",
		DRAWTEXT_EDITABLE, TRUE,
		DRAWTEXT_LENGTH, 8,
		DRAWTEXT_JUSTIFY, DRAWTEXT_JUSTIFY_CENTER,
		RECTOBJ_START_DRAG_PROC, start_drag_proc,
		0);

	c = (Clockobj) xv_create(bag, CLOCKOBJ,
		XV_X, 160,
		XV_Y, 160,
		XV_WIDTH, 35,
		XV_HEIGHT, 35,
		CLOCKOBJ_HR, 3,
		CLOCKOBJ_MOVABLE, FALSE,
		RECTOBJ_SELECTION_PROC, highlight_clockobj,
		RECTOBJ_SELECTABLE, TRUE,
		RECTOBJ_START_DRAG_PROC, start_drag_proc,
		0);

	/*
	 * Make sure no events get mapped to the clock hands
	 * by turning off their event procs
	 */
	xv_set(xv_get(c, CLOCKOBJ_HR_DRAWAREA),
		RECTOBJ_MAP_EVENT_PROC, 0,
		0);

	xv_set(xv_get(c, CLOCKOBJ_MIN_DRAWAREA),
		RECTOBJ_MAP_EVENT_PROC, 0,
		0);

	(void) xv_create(bag, TACHO,
		XV_X, 200,
		XV_Y, 200,
		XV_WIDTH, 35,
		XV_HEIGHT, 26,
		TACHO_VALUE, 40,
		RECTOBJ_SELECTABLE, TRUE,
		RECTOBJ_START_DRAG_PROC, start_drag_proc,
		0);

	/* size the canvas to the size of the bag plus some play room */
	xv_set(shell, 
		XV_WIDTH, xv_get(bag, XV_WIDTH) + 10,
		XV_HEIGHT, xv_get(bag, XV_HEIGHT) + 10,
		0);

	window_fit(frame);

	xv_main_loop(frame);

	xv_destroy( smiley );
	xv_destroy( frown );
	xv_destroy( ampersand );
}


void
highlight_clockobj(Clockobj clockobj, int add, Event	* event)
{
	/* make some visible change so that we know the clock is selected */
	VClear(clockobj);
	if(add)
		VSetLineWidth(clockobj, 2);
	VDrawArc(clockobj, 500, 500, 9500, 9500, 0, 360*64);
	rectobj_repaint_rect(clockobj, 0, TRUE);
}


Rectobj movee;

int
move_proc(Xv_window paint_window, Event          * event, Canvas_shell canvas_shell, Grip grip, short          * new_x, short          * new_y)
{
	/* 
	 * At this point, we might test to see if the object is within the window
	 * and if not, start a dnd operation.
	 */

	xv_set(movee,
		XV_X, *new_x,
		XV_Y, *new_y,
		0);

	return TRUE;
}

void
done_proc(Xv_window paint_window, Event          * event, Canvas_shell canvas_shell, Grip grip, int last_x, int last_y)
{
	/* turn this off when finished because pixmap space is precious */
	xv_set(canvas_shell,
		CANVAS_SHELL_BATCH_REPAINT, FALSE,
		0);
}


void
start_drag_proc(Xv_window paint_window, Event		* event, Canvas_shell canvas_shell, Rectobj object, int btn_down_x, int btn_down_y, int adjust)
{
	/* turn this on while dragging so things look nice. */
	xv_set(canvas_shell,
		CANVAS_SHELL_BATCH_REPAINT, TRUE,
		0);
	movee = object;
	(void) xv_create(canvas_shell, TEMP_GRIP, 
		GRIP_MOVE_PROC, move_proc,
		GRIP_DONE_PROC, done_proc,
		0);
}

