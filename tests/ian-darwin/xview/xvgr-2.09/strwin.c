/* $Id: strwin.c,v 1.9 92/07/19 08:14:00 pturner Exp Locker: pturner $
 *
 * strings, lines, and boxes
 *
 */

#include <stdio.h>
#include <math.h>
#include <sys/types.h>

#include <xview/xview.h>
#include <xview/frame.h>
#include <xview/canvas.h>
#include <xview/panel.h>
#include <xview/font.h>
#include <xview/cursor.h>

#include "globals.h"

extern Frame main_frame;

/*
extern Xv_cursor cursor_box, cursor_line, cursor_del, cursor_move, cursor_strloc;
*/

void define_objects_popup();
void define_strings_popup();
void define_lines_popup();
void define_boxes_popup();

Frame strings_frame;
static Panel strings_panel;

Frame lines_frame;
static Panel lines_panel;

Frame boxes_frame;
static Panel boxes_panel;

static Panel_item strings_font_item;
static Panel_item strings_rot_item;
static Panel_item strings_size_item;
static Panel_item strings_loc_item;
static Panel_item strings_pen_item;
static Panel_item strings_linew_item;
static Panel_item strings_just_item;
Panel_item strings_x_item;
Panel_item strings_y_item;

static Panel_item lines_arrow_item;
static Panel_item lines_arrowhead_item;
static Panel_item lines_asize_item;
static Panel_item lines_pen_item;
static Panel_item lines_style_item;
static Panel_item lines_width_item;
static Panel_item lines_loc_item;
static Panel_item boxes_pen_item;
static Panel_item boxes_lines_item;
static Panel_item boxes_linew_item;
static Panel_item boxes_fill_item;
static Panel_item boxes_fillpat_item;
static Panel_item boxes_fillcol_item;
static Panel_item boxes_loc_item;

/* the following are defined in objutils.c */
void do_boxes_proc();
void do_lines_proc();
void do_move_proc();
void do_delete_object_proc();
void strings_loc_proc();
void strings_ang_proc();
void strings_edit_proc();
void do_clear_lines();
void do_clear_boxes();
void do_clear_text();

void define_string_defaults(void)
{
    int iv;

    if (strings_frame) {
	string_font = (int) xv_get(strings_font_item, PANEL_VALUE);
	string_color = (int) xv_get(strings_pen_item, PANEL_VALUE);
	string_linew = (int) xv_get(strings_linew_item, PANEL_VALUE) + 1;
	iv = (int) xv_get(strings_size_item, PANEL_VALUE);
	string_size = iv / 100.0;
	iv = (int) xv_get(strings_rot_item, PANEL_VALUE);
	string_rot = iv;
	string_loctype = (int) xv_get(strings_loc_item, PANEL_VALUE) ? VIEW : WORLD;
	string_just = (int) xv_get(strings_just_item, PANEL_VALUE);
    }
}

void boxes_def_proc(void)
{
    box_color = (int) xv_get(boxes_pen_item, PANEL_VALUE);
    box_loctype = (int) xv_get(boxes_loc_item, PANEL_VALUE) ? VIEW : WORLD;
    box_lines = (int) xv_get(boxes_lines_item, PANEL_VALUE) + 1;
    box_linew = (int) xv_get(boxes_linew_item, PANEL_VALUE) + 1;
    switch (xv_get(boxes_fill_item, PANEL_VALUE)) {
    case 0:
	box_fill = NONE;
	break;
    case 1:
	box_fill = COLOR;
	break;
    case 2:
	box_fill = PATTERN;
	break;
    }
    box_fillcolor = (int) xv_get(boxes_fillcol_item, PANEL_VALUE);
    box_fillpat = (int) xv_get(boxes_fillpat_item, PANEL_VALUE) + 1;
}

void lines_def_proc(void)
{
    int value;

    value = (int) xv_get(lines_asize_item, PANEL_VALUE);
    line_asize = value / 50.0;
    line_color = (int) xv_get(lines_pen_item, PANEL_VALUE);
    line_arrow = (int) xv_get(lines_arrow_item, PANEL_VALUE);
    line_atype = (int) xv_get(lines_arrowhead_item, PANEL_VALUE);
    line_lines = (int) xv_get(lines_style_item, PANEL_VALUE) + 1;
    line_linew = (int) xv_get(lines_width_item, PANEL_VALUE) + 1;
    line_loctype = (int) xv_get(lines_loc_item, PANEL_VALUE) ? VIEW : WORLD;
}

void updatestrings(void)
{
    int iv;

    if (strings_frame) {
	xv_set(strings_font_item, PANEL_VALUE, string_font, 0);
	xv_set(strings_pen_item, PANEL_VALUE, string_color, 0);
	xv_set(strings_linew_item, PANEL_VALUE, string_linew - 1, 0);
	iv = (int) (100 * string_size);
	xv_set(strings_size_item, PANEL_VALUE, iv, 0);
	xv_set(strings_rot_item, PANEL_VALUE, string_rot, 0);
	xv_set(strings_loc_item, PANEL_VALUE, string_loctype == VIEW ? 1 : 0, 0);
	xv_set(strings_just_item, PANEL_VALUE, string_just, 0);
    }
}

void update_lines(void)
{
    int iv;

    if (lines_frame) {
	xv_set(lines_pen_item, PANEL_VALUE, line_color, 0);
	xv_set(lines_style_item, PANEL_VALUE, line_lines - 1, 0);
	xv_set(lines_width_item, PANEL_VALUE, line_linew - 1, 0);
	xv_set(lines_arrow_item, PANEL_VALUE, line_arrow, 0);
	xv_set(lines_arrowhead_item, PANEL_VALUE, line_atype, 0);
	iv = (int) (50 * line_asize);
	xv_set(lines_asize_item, PANEL_VALUE, iv, 0);
	xv_set(lines_loc_item, PANEL_VALUE, line_loctype == VIEW ? 1 : 0, 0);
    }
}

void update_boxes(void)
{
    if (boxes_frame) {
	xv_set(boxes_pen_item, PANEL_VALUE, box_color, 0);
	xv_set(boxes_lines_item, PANEL_VALUE, box_lines - 1, 0);
	xv_set(boxes_linew_item, PANEL_VALUE, box_linew - 1, 0);
	switch (box_fill) {
	case NONE:
	    xv_set(boxes_fill_item, PANEL_VALUE, 0, 0);
	    break;
	case COLOR:
	    xv_set(boxes_fill_item, PANEL_VALUE, 1, 0);
	    break;
	case PATTERN:
	    xv_set(boxes_fill_item, PANEL_VALUE, 2, 0);
	    break;
	}
	xv_set(boxes_fillpat_item, PANEL_VALUE, box_fillpat - 1, 0);
	xv_set(boxes_fillcol_item, PANEL_VALUE, box_fillcolor, 0);
	xv_set(boxes_loc_item, PANEL_VALUE, box_loctype == VIEW ? 1 : 0, 0);
    }
}

static void strings_done_proc(void)
{
    xv_set(strings_frame, WIN_SHOW, FALSE, 0);
}

static void lines_done_proc(void)
{
    xv_set(lines_frame, WIN_SHOW, FALSE, 0);
}

static void boxes_done_proc(void)
{
    xv_set(boxes_frame, WIN_SHOW, FALSE, 0);
}

void define_strings_popup(void)
{
    if (strings_frame) {
	updatestrings();
	xv_set(strings_frame, WIN_SHOW, TRUE, 0);
	return;
    }
    strings_frame = xv_create(main_frame, FRAME,
			      XV_LABEL, "Strings",
			      FRAME_SHOW_LABEL, TRUE,
			      WIN_ERROR_MSG, "Couldn't create strings_frame",
			      0);
    strings_panel = xv_create(strings_frame, PANEL,
			      PANEL_LAYOUT, PANEL_VERTICAL,
			      0);

    strings_font_item = (Panel_item) xv_create(strings_panel, PANEL_CHOICE_STACK,
					       PANEL_LABEL_STRING, "Font:",
					       PANEL_CHOICE_STRINGS,
				"Times-Roman", "Times-Bold", "Times-Italic",
					    "Times-BoldItalic", "Helvetica",
				      "Helvetica-Bold", "Helvetica-Oblique",
				 "Helvetica-BoldOblique", "Greek", "Symbol",
					       0,
				   PANEL_VALUE_X, xv_col(strings_panel, 16),
					       0);

    strings_pen_item = (Panel_item) xv_create(strings_panel, PANEL_CHOICE_STACK,
					      PANEL_LABEL_STRING, "Color:",
					      PANEL_CHOICE_NCOLS, 4,
					      PANEL_CHOICE_STRINGS,
			   "0", "1", "2", "3", "4", "5", "6", "7", "8", "9",
					 "10", "11", "12", "13", "14", "15",
					      0,
				   PANEL_VALUE_X, xv_col(strings_panel, 16),
					      0);

    strings_linew_item = (Panel_item) xv_create(strings_panel, PANEL_CHOICE_STACK,
					  PANEL_LABEL_STRING, "Line width:",
						PANEL_CHOICE_NCOLS, 3,
						PANEL_CHOICE_STRINGS,
				"1", "2", "3", "4", "5", "6", "7", "8", "9",
						0,
				   PANEL_VALUE_X, xv_col(strings_panel, 16),
						0);

    strings_just_item = (Panel_item) xv_create(strings_panel, PANEL_CHOICE_STACK,
				       PANEL_LABEL_STRING, "Justification:",
					       PANEL_CHOICE_STRINGS,
					       "Left",
					       "Right",
					       "Centered",
					       0,
				   PANEL_VALUE_X, xv_col(strings_panel, 16),
					       0);

    strings_loc_item = (Panel_item) xv_create(strings_panel, PANEL_CHOICE_STACK,
					 PANEL_LABEL_STRING, "Position in:",
					      PANEL_CHOICE_STRINGS,
					      "World coordinates",
					      "Viewport coordinates",
					      0,
				   PANEL_VALUE_X, xv_col(strings_panel, 16),
					      0);

    strings_rot_item = (Panel_item) xv_create(strings_panel, PANEL_SLIDER,
					      PANEL_SLIDER_WIDTH, 160,
					      PANEL_SHOW_VALUE, TRUE,
					      PANEL_SHOW_RANGE, FALSE,
					      PANEL_MIN_VALUE, 0,
					      PANEL_MAX_VALUE, 360,
					    PANEL_LABEL_STRING, "Rotation:",
				   PANEL_VALUE_X, xv_col(strings_panel, 16),
					      0);

    strings_size_item = (Panel_item) xv_create(strings_panel, PANEL_SLIDER,
					       PANEL_SLIDER_WIDTH, 100,
					       PANEL_SHOW_VALUE, TRUE,
					       PANEL_SHOW_RANGE, FALSE,
					       PANEL_MIN_VALUE, 0,
					       PANEL_MAX_VALUE, 400,
				      PANEL_LABEL_STRING, "Character size:",
				   PANEL_VALUE_X, xv_col(strings_panel, 16),
					       0);
    (void) xv_create(strings_panel, PANEL_BUTTON,
		     PANEL_LABEL_STRING, "Done",
		     PANEL_NOTIFY_PROC, strings_done_proc,
		     XV_X, xv_col(strings_panel, 15),
		     XV_Y, xv_row(strings_panel, 8),
		     0);
    (void) xv_create(strings_panel, PANEL_BUTTON,
		     PANEL_LABEL_STRING, "Accept",
		     PANEL_NOTIFY_PROC, define_string_defaults,
		     XV_X, xv_col(strings_panel, 5),
		     XV_Y, xv_row(strings_panel, 8),
		     0);
    updatestrings();
    window_fit(strings_panel);
    window_fit(strings_frame);
    xv_set(strings_frame, WIN_SHOW, TRUE, 0);
}

void define_lines_popup(void)
{
    if (lines_frame) {
	update_lines();
	xv_set(lines_frame, WIN_SHOW, TRUE, 0);
	return;
    }
    lines_frame = xv_create(main_frame, FRAME,
			    XV_LABEL, "Lines",
			    FRAME_SHOW_LABEL, TRUE,
			    WIN_ERROR_MSG, "Couldn't create lines_frame",
			    0);
    lines_panel = xv_create(lines_frame, PANEL,
			    PANEL_LAYOUT, PANEL_VERTICAL,
			    0);

    lines_pen_item = (Panel_item) xv_create(lines_panel, PANEL_CHOICE_STACK,
					    PANEL_LABEL_STRING, "Color:",
					    PANEL_CHOICE_NCOLS, 4,
					    PANEL_CHOICE_STRINGS,
			   "0", "1", "2", "3", "4", "5", "6", "7", "8", "9",
					 "10", "11", "12", "13", "14", "15",
					    0,
				     PANEL_VALUE_X, xv_col(lines_panel, 20),
					    0);

    lines_width_item = (Panel_item) xv_create(lines_panel, PANEL_CHOICE_STACK,
					  PANEL_LABEL_STRING, "Line width:",
					      PANEL_CHOICE_NCOLS, 3,
					      PANEL_CHOICE_STRINGS,
				"1", "2", "3", "4", "5", "6", "7", "8", "9",
					      0,
				     PANEL_VALUE_X, xv_col(lines_panel, 20),
					      0);

    lines_style_item = (Panel_item) xv_create(lines_panel, PANEL_CHOICE_STACK,
					  PANEL_LABEL_STRING, "Line style:",
					      PANEL_CHOICE_STRINGS,
					      "Solid line",
					      "Dotted line",
					      "Dashed line",
					      "Long Dashed",
					      "Dot-dashed",
					      0,
				     PANEL_VALUE_X, xv_col(lines_panel, 20),
					      0);
    lines_arrow_item = (Panel_item) xv_create(lines_panel, PANEL_CHOICE_STACK,
					      PANEL_LABEL_STRING, "Arrow:",
					      PANEL_CHOICE_STRINGS,
					      "None",
					      "At start",
					      "At end",
					      "Both ends",
					      0,
				     PANEL_VALUE_X, xv_col(lines_panel, 20),
					      0);

    lines_arrowhead_item = (Panel_item) xv_create(lines_panel, PANEL_CHOICE_STACK,
				    PANEL_LABEL_STRING, "Arrow head style:",
						  PANEL_CHOICE_STRINGS,
						  "Line",
						  "Filled",
						  "Hollow",
						  0,
				     PANEL_VALUE_X, xv_col(lines_panel, 20),
						  0);

    lines_loc_item = (Panel_item) xv_create(lines_panel, PANEL_CHOICE_STACK,
					 PANEL_LABEL_STRING, "Position in:",
					    PANEL_CHOICE_STRINGS,
					    "World coordinates",
					    "Viewport coordinates",
					    0,
				     PANEL_VALUE_X, xv_col(lines_panel, 20),
					    0);

    lines_asize_item = (Panel_item) xv_create(lines_panel, PANEL_SLIDER,
					      PANEL_SLIDER_WIDTH, 100,
					      PANEL_SHOW_VALUE, TRUE,
					      PANEL_SHOW_RANGE, FALSE,
					      PANEL_MIN_VALUE, 0,
					      PANEL_MAX_VALUE, 400,
					  PANEL_LABEL_STRING, "Arrow size:",
				     PANEL_VALUE_X, xv_col(lines_panel, 20),
					      0);
    (void) xv_create(lines_panel, PANEL_BUTTON,
		     PANEL_LABEL_STRING, "Done",
		     PANEL_NOTIFY_PROC, lines_done_proc,
		     XV_X, xv_col(lines_panel, 15),
		     XV_Y, xv_row(lines_panel, 9),
		     0);
    (void) xv_create(lines_panel, PANEL_BUTTON,
		     PANEL_LABEL_STRING, "Accept",
		     PANEL_NOTIFY_PROC, lines_def_proc,
		     XV_X, xv_col(lines_panel, 5),
		     XV_Y, xv_row(lines_panel, 9),
		     0);
    update_lines();
    window_fit(lines_panel);
    window_fit(lines_frame);
    xv_set(lines_frame, WIN_SHOW, TRUE, 0);
}

void define_boxes_popup(void)
{
    if (boxes_frame) {
	update_boxes();
	xv_set(boxes_frame, WIN_SHOW, TRUE, 0);
	return;
    }
    boxes_frame = xv_create(main_frame, FRAME,
			    XV_LABEL, "Boxes",
			    FRAME_SHOW_LABEL, TRUE,
			    WIN_ERROR_MSG, "Couldn't create boxes_frame",
			    0);
    boxes_panel = xv_create(boxes_frame, PANEL,
			    PANEL_LAYOUT, PANEL_VERTICAL,
			    0);

    boxes_pen_item = (Panel_item) xv_create(boxes_panel, PANEL_CHOICE_STACK,
					    PANEL_LABEL_STRING, "Color:",
					    PANEL_CHOICE_NCOLS, 4,
					    PANEL_CHOICE_STRINGS,
			   "0", "1", "2", "3", "4", "5", "6", "7", "8", "9",
					 "10", "11", "12", "13", "14", "15",
					    0,
				     PANEL_VALUE_X, xv_col(boxes_panel, 16),
					    0);

    boxes_linew_item = (Panel_item) xv_create(boxes_panel, PANEL_CHOICE_STACK,
					  PANEL_LABEL_STRING, "Line width:",
					      PANEL_CHOICE_NCOLS, 3,
					      PANEL_CHOICE_STRINGS,
				"1", "2", "3", "4", "5", "6", "7", "8", "9",
					      0,
				     PANEL_VALUE_X, xv_col(boxes_panel, 16),
					      0);

    boxes_lines_item = (Panel_item) xv_create(boxes_panel, PANEL_CHOICE_STACK,
					  PANEL_LABEL_STRING, "Line style:",
					      PANEL_CHOICE_STRINGS,
					      "Solid line",
					      "Dotted line",
					      "Dashed line",
					      "Long Dashed",
					      "Dot-dashed",
					      0,
				     PANEL_VALUE_X, xv_col(boxes_panel, 16),
					      0);
    boxes_fill_item = (Panel_item) xv_create(boxes_panel, PANEL_CHOICE_STACK,
					     PANEL_LABEL_STRING, "Fill:",
					     PANEL_CHOICE_STRINGS,
					     "None",
					     "Color",
					     "Pattern",
					     0,
				     PANEL_VALUE_X, xv_col(boxes_panel, 16),
					     0);
    boxes_fillpat_item = (Panel_item) xv_create(boxes_panel, PANEL_CHOICE_STACK,
					     PANEL_LABEL_STRING, "Pattern:",
						PANEL_CHOICE_NCOLS, 4,
						PANEL_CHOICE_STRINGS,
			   "0", "1", "2", "3", "4", "5", "6", "7", "8", "9",
					 "10", "11", "12", "13", "14", "15",
						0,
				     PANEL_VALUE_X, xv_col(boxes_panel, 16),
						0);

    boxes_fillcol_item = (Panel_item) xv_create(boxes_panel, PANEL_CHOICE_STACK,
						PANEL_LABEL_STRING, "Color:",
						PANEL_CHOICE_NCOLS, 4,
						PANEL_CHOICE_STRINGS,
			   "0", "1", "2", "3", "4", "5", "6", "7", "8", "9",
					 "10", "11", "12", "13", "14", "15",
						0,
				     PANEL_VALUE_X, xv_col(boxes_panel, 16),
						0);

    boxes_loc_item = (Panel_item) xv_create(boxes_panel, PANEL_CHOICE_STACK,
					 PANEL_LABEL_STRING, "Position in:",
					    PANEL_CHOICE_STRINGS,
					    "World coordinates",
					    "Viewport coordinates",
					    0,
				     PANEL_VALUE_X, xv_col(boxes_panel, 16),
					    0);
    (void) xv_create(boxes_panel, PANEL_BUTTON,
		     PANEL_LABEL_STRING, "Done",
		     PANEL_NOTIFY_PROC, boxes_done_proc,
		     XV_X, xv_col(boxes_panel, 15),
		     XV_Y, xv_row(boxes_panel, 9),
		     0);
    (void) xv_create(boxes_panel, PANEL_BUTTON,
		     PANEL_LABEL_STRING, "Accept",
		     PANEL_NOTIFY_PROC, boxes_def_proc,
		     XV_X, xv_col(boxes_panel, 5),
		     XV_Y, xv_row(boxes_panel, 9),
		     0);
    update_boxes();
    window_fit(boxes_panel);
    window_fit(boxes_frame);
    xv_set(boxes_frame, WIN_SHOW, TRUE, 0);
}
