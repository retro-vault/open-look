/* $Id: ptswin.c,v 1.6 92/06/10 08:46:42 pturner Exp Locker: pturner $
 *
 * edit points, tracker
 *
 */

#include <stdio.h>
#include <math.h>

#include <xview/xview.h>
#include <xview/frame.h>
#include <xview/canvas.h>
#include <xview/panel.h>
#include <xview/font.h>
#include <xview/cursor.h>

#include "globals.h"

extern Frame main_frame;

extern int add_setno;

int track_set = 0;
int track_point = 0;
int paint_skip = 0;

int move_dir = 0;

void drawgraph();
void define_points_popup();
void create_points_frame();

Frame points_frame = (Frame) 0;
Panel points_panel;

Frame goto_frame = (Frame) 0;
Panel goto_panel;

Frame add_frame = (Frame) 0;
Panel add_panel;

Panel_item locate_point_item;
Panel_item locate_point_message;
Panel_item goto_pointx_item;
Panel_item goto_pointy_item;

Panel_item addinset_item;
Panel_item paint_points_item;
Panel_item paint_skip_item;

void do_find_points();
void do_track_points();
void do_delete_points();
void create_add_frame();
void do_move_points();
void create_goto_frame();

/*
 * set tracker
 */
char * xv_getstr(Panel_item p);
int activeset(int gno);
int world2deviceabs(double wx, double wy, int * x, int *y);
void errwin(char * msg);
void getpoints(int x, int y);
void set_action(int act);
void setpointer(int x, int y);

void do_track_points(void)
{
    set_action(0);
    if (activeset(cg)) {
	set_action(TRACKER);
	create_points_frame();
	xv_set(locate_point_message, PANEL_LABEL_STRING, "Tracking -  Set, location, (X, Y):", 0);
	track_set = -1;
    } else {
	errwin("No active sets to track");
    }
}

/*
 * activate the add point item in the canvas event proc
 */
void do_add_points(void)
{
    int which = (int) xv_get(paint_points_item, PANEL_VALUE);

    set_action(0);
    if (which) {
	set_action(PAINT_POINTS);
	/* negate paint_skip to signal initialization */
	paint_skip = (int) xv_get(paint_skip_item, PANEL_VALUE);
	if (paint_skip > 0) {
	    paint_skip = -paint_skip;
	}
    } else {
	set_action(ADD_POINT);
    }
    add_setno = (int) xv_get(addinset_item, PANEL_VALUE);
    create_points_frame();
    xv_set(add_frame, WIN_SHOW, FALSE, 0);
    if (which) {
	xv_set(locate_point_message, PANEL_LABEL_STRING, "Painting points to set, location, (X, Y):", 0);
    } else {
	xv_set(locate_point_message, PANEL_LABEL_STRING, "Adding points to set, location, (X, Y):", 0);
    }
}

/*
 * activate the find point item in the canvas event proc
 */
void do_find_points(void)
{
    set_action(0);
    set_action(FIND_POINT);
    create_points_frame();
    xv_set(locate_point_message, PANEL_LABEL_STRING, "Set, location, (X, Y):", 0);
}

/*
 * activate the delete point item in the canvas event proc
 */
void do_delete_points(void)
{
    set_action(0);
    set_action(DEL_POINT);
    create_points_frame();
    xv_set(locate_point_message, PANEL_LABEL_STRING, "Delete points - set, location, (X, Y):", 0);
}

/*
 * move a point
 */
void do_move_points(void)
{
    set_action(0);
    set_action(MOVE_POINT1ST);
    move_dir = 0;
    create_points_frame();
    xv_set(locate_point_message, PANEL_LABEL_STRING, "Move points - set, location, (X, Y):", 0);
}

void do_movey_points(void)
{
    set_action(0);
    set_action(MOVE_POINT1ST);
    move_dir = 2;
    create_points_frame();
    xv_set(locate_point_message, PANEL_LABEL_STRING, "Move points along y only- set, location, (X, Y):", 0);
}

void do_movex_points(void)
{
    set_action(0);
    set_action(MOVE_POINT1ST);
    move_dir = 1;
    create_points_frame();
    xv_set(locate_point_message, PANEL_LABEL_STRING, "Move points along x only - set, location, (X, Y):", 0);
}

/* ARGSUSED */
static void do_goto_proc(Panel_item item, Event *event)
{
    double wx, wy;
    int sx, sy;

    wx = atof((char *) xv_getstr(goto_pointx_item));
    wy = atof((char *) xv_getstr(goto_pointy_item));
    world2deviceabs(wx, wy, &sx, &sy);
    setpointer(sx, sy);
    getpoints(sx, sy);
}

static void points_done_proc(void)
{
    xv_set(points_frame, WIN_SHOW, FALSE, 0);
}

void create_points_frame(void)
{
    if (points_frame) {
	xv_set(points_frame, WIN_SHOW, TRUE, 0);
	return;
    }
    points_frame = xv_create(main_frame, FRAME,
			     XV_LABEL, "Points",
			     FRAME_SHOW_LABEL, TRUE,
			     WIN_ERROR_MSG, "Couldn't create points_frame",
			     WIN_Y, 0,
			     0);
    points_panel = xv_create(points_frame, PANEL,
			     PANEL_LAYOUT, PANEL_VERTICAL,
			     0);
    locate_point_message = xv_create(points_panel, PANEL_MESSAGE,
			       PANEL_LABEL_STRING, "Set, location, (X, Y):",
				     0);
    locate_point_item = (Panel_item) xv_create(points_panel, PANEL_TEXT,
					     PANEL_LAYOUT, PANEL_HORIZONTAL,
					     PANEL_VALUE_DISPLAY_LENGTH, 40,
					       0);
    (void) xv_create(points_panel, PANEL_BUTTON,
		     PANEL_LABEL_STRING, "Done",
		     PANEL_NOTIFY_PROC, points_done_proc,
		     0);

    window_fit(points_panel);
    window_fit(points_frame);
    xv_set(points_frame, WIN_SHOW, TRUE, 0);
}

static void goto_done_proc(void)
{
    xv_set(goto_frame, WIN_SHOW, FALSE, 0);
}

void create_goto_frame(void)
{
    if (goto_frame) {
	xv_set(goto_frame, WIN_SHOW, TRUE, 0);
	return;
    }
    goto_frame = xv_create(main_frame, FRAME,
			   XV_LABEL, "Goto point",
			   FRAME_SHOW_LABEL, TRUE,
			   WIN_ERROR_MSG, "Couldn't create goto_frame",
			   WIN_Y, 0,
			   0);
    goto_panel = xv_create(goto_frame, PANEL,
			   PANEL_LAYOUT, PANEL_VERTICAL,
			   0);
    goto_pointx_item = (Panel_item) xv_create(goto_panel, PANEL_TEXT,
					      PANEL_LAYOUT, PANEL_HORIZONTAL,
					      PANEL_VALUE_DISPLAY_LENGTH, 10,
					      PANEL_LABEL_STRING, "X:",
				       PANEL_VALUE_X, xv_col(goto_panel, 5),
					      0);
    goto_pointy_item = (Panel_item) xv_create(goto_panel, PANEL_TEXT,
					      PANEL_LAYOUT, PANEL_HORIZONTAL,
					      PANEL_VALUE_DISPLAY_LENGTH, 10,
					      PANEL_LABEL_STRING, "Y:",
				       PANEL_VALUE_X, xv_col(goto_panel, 5),
					      0);
    (void) xv_create(goto_panel, PANEL_BUTTON,
		     PANEL_LABEL_STRING, "Goto point",
		     PANEL_NOTIFY_PROC, do_goto_proc,
		     XV_X, xv_col(goto_panel, 2),
		     XV_Y, xv_row(goto_panel, 2),
		     0);
    (void) xv_create(goto_panel, PANEL_BUTTON,
		     PANEL_LABEL_STRING, "Done",
		     PANEL_NOTIFY_PROC, goto_done_proc,
		     XV_X, xv_col(goto_panel, 15),
		     XV_Y, xv_row(goto_panel, 2),
		     0);

    window_fit(goto_panel);
    window_fit(goto_frame);
    xv_set(goto_frame, WIN_SHOW, TRUE, 0);
}

static void add_done_proc(void)
{
    xv_set(add_frame, WIN_SHOW, FALSE, 0);
}

void create_add_frame(void)
{
    if (add_frame) {
	xv_set(add_frame, WIN_SHOW, TRUE, 0);
	return;
    }
    add_frame = xv_create(main_frame, FRAME,
			  XV_LABEL, "Add points",
			  FRAME_SHOW_LABEL, TRUE,
			  WIN_ERROR_MSG, "Couldn't create add_frame",
			  WIN_Y, 0,
			  0);
    add_panel = xv_create(add_frame, PANEL,
			  PANEL_LAYOUT, PANEL_VERTICAL,
			  0);
    define_select_set_panel0(add_panel, addinset_item, "Add points to set:", xv_col(add_panel, 0), xv_row(add_panel, 0));
    paint_points_item = (Panel_item) xv_create(add_panel, PANEL_CHECK_BOX,
					     XV_HELP_DATA, "xvgr:add_paint",
					       PANEL_CHOICE_STRINGS,
					       "Paint points",
					       0,
				       PANEL_VALUE_X, xv_col(add_panel, 15),
					       XV_Y, xv_row(add_panel, 1),
					       0);
    paint_skip_item = (Panel_item) xv_create(add_panel, PANEL_NUMERIC_TEXT,
					     XV_HELP_DATA, "xvgr:add_skip",
					     PANEL_LABEL_STRING, "Skip:",
					     PANEL_VALUE_DISPLAY_LENGTH, 4,
					     PANEL_MIN_VALUE, 0,
					     PANEL_MAX_VALUE, 500,
				       PANEL_VALUE_X, xv_col(add_panel, 15),
					     XV_Y, xv_row(add_panel, 2),
					     0);

    (void) xv_create(add_panel, PANEL_BUTTON,
		     PANEL_LABEL_STRING, "Add points",
		     PANEL_NOTIFY_PROC, do_add_points,
		     XV_X, xv_col(add_panel, 2),
		     XV_Y, xv_row(add_panel, 3),
		     0);
    (void) xv_create(add_panel, PANEL_BUTTON,
		     PANEL_LABEL_STRING, "Done",
		     PANEL_NOTIFY_PROC, add_done_proc,
		     XV_X, xv_col(add_panel, 15),
		     XV_Y, xv_row(add_panel, 3),
		     0);
    window_fit(add_panel);
    window_fit(add_frame);
    xv_set(add_frame, WIN_SHOW, TRUE, 0);
}
