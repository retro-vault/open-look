/* $Id: regionwin.c,v 1.6 92/02/15 17:36:25 pturner Exp Locker: pturner $
 *
 * define regions and operate on regions
 */
#include <stdio.h>
#include <math.h>

#include <xview/xview.h>
#include <xview/frame.h>
#include <xview/panel.h>

#include "globals.h"

extern Frame main_frame;

#define FRAME_KEY        101
void generic_done_proc();

void create_define_frame();
void create_clear_frame();
void create_extract_frame();
void create_delete_frame();
void create_evalregion_frame();
void create_area_frame();

void do_select_region();
void do_eval_region();

extern int regiontype, regionlinkto;

Panel_item define_region_item;
Panel_item define_type_item;
Panel_item define_linkto_item;

int isactive_region(int regno);
int nextset(int gno);
int yesno(char * msg1, char *msg2, char * msgyes, char *msgno);
void define_region(int nr, int regionlinkto, int rtype);
void delete_region(int gno, int regno);
void drawgraph(void);
void errwin(char * msg);
void evaluate_region(int regno, char * buf);
void extract_region(int gno, int setno, int regno);
void fixupstr(char * val);
void kill_region(int r);

int do_define_region(void)
{
    int rtype = xv_get(define_type_item, PANEL_VALUE);

    nr = xv_get(define_region_item, PANEL_VALUE);
    if (isactive_region(nr)) {
	if (!yesno("Region is active, kill it?", " ", "YES", "CANCEL")) {
	    return 0;
	}
    }
    regionlinkto = xv_get(define_linkto_item, PANEL_VALUE);
    define_region(nr, regionlinkto, rtype);
}

void create_define_frame(void)
{
    static Frame top;
    static Panel dialog;

    if (top) {
	xv_set(top, WIN_SHOW, TRUE, 0);
	return;
    }
    top = (Frame) xv_create(main_frame, FRAME,
			    FRAME_LABEL, "Define region",
			    0);
    dialog = (Panel) xv_create(top, PANEL,
			       PANEL_LAYOUT, PANEL_VERTICAL,
			       0);

    define_region_item = xv_create(dialog, PANEL_CYCLE,
				   PANEL_LABEL_STRING, "Define region:",
				   PANEL_CHOICE_STRINGS,
				   "0", "1", "2", "3", "4",
				   0,
				   PANEL_VALUE_X, xv_col(dialog, 18),
				   0);

    define_type_item = xv_create(dialog, PANEL_CYCLE,
				 PANEL_LABEL_STRING, "Region type:",
				 PANEL_CHOICE_STRINGS,
				 "Inside polygon",
				 "Outside polygon",
				 "Above line",
				 "Below line",
				 "Left of line",
				 "Right of line",
				 0,
				 PANEL_VALUE_X, xv_col(dialog, 18),
				 0);

    define_linkto_item = xv_create(dialog, PANEL_CYCLE,
				   PANEL_LABEL_STRING, "Link to graph(s):",
				   PANEL_CHOICE_STRINGS,
				   "Current",
				   "All",
				   0,
				   PANEL_VALUE_X, xv_col(dialog, 18),
				   0);
    (void) xv_create(dialog, PANEL_BUTTON,
		     PANEL_LABEL_STRING, "Apply",
		     PANEL_NOTIFY_PROC, do_define_region,
		     XV_X, xv_col(dialog, 5),
		     XV_Y, xv_row(dialog, 4),
		     0);
    (void) xv_create(dialog, PANEL_BUTTON,
		     PANEL_LABEL_STRING, "Done",
		     PANEL_NOTIFY_PROC, generic_done_proc,
		     XV_KEY_DATA, FRAME_KEY, top,
		     XV_X, xv_col(dialog, 15),
		     XV_Y, xv_row(dialog, 4),
		     0);
    window_fit(dialog);
    window_fit(top);
    xv_set(top, WIN_SHOW, TRUE, 0);
}

Panel_item clear_region_item;

void do_clear_region(void)
{
    int i;

    if ((int) xv_get(clear_region_item, PANEL_VALUE) == MAXREGION) {
	for (i = 0; i < MAXREGION; i++) {
	    kill_region(i);
	}
    } else {
	kill_region((int) xv_get(clear_region_item, PANEL_VALUE));
    }
    drawgraph();
}

void create_clear_frame(void)
{
    static Frame top;
    static Panel dialog;

    if (top) {
	xv_set(top, WIN_SHOW, TRUE, 0);
	return;
    }
    top = (Frame) xv_create(main_frame, FRAME,
			    FRAME_LABEL, "Clear region",
			    0);
    dialog = (Panel) xv_create(top, PANEL,
			       PANEL_LAYOUT, PANEL_VERTICAL,
			       0);

    clear_region_item = xv_create(dialog, PANEL_CYCLE,
				  PANEL_LABEL_STRING, "Clear region:",
				  PANEL_CHOICE_STRINGS,
				  "0", "1", "2", "3", "4", "All",
				  0,
				  0);

    (void) xv_create(dialog, PANEL_BUTTON,
		     PANEL_LABEL_STRING, "Apply",
		     PANEL_NOTIFY_PROC, do_clear_region,
		     XV_X, xv_col(dialog, 5),
		     XV_Y, xv_row(dialog, 2),
		     0);
    (void) xv_create(dialog, PANEL_BUTTON,
		     PANEL_LABEL_STRING, "Done",
		     PANEL_NOTIFY_PROC, generic_done_proc,
		     XV_KEY_DATA, FRAME_KEY, top,
		     XV_X, xv_col(dialog, 15),
		     XV_Y, xv_row(dialog, 2),
		     0);
    window_fit(dialog);
    window_fit(top);
    xv_set(top, WIN_SHOW, TRUE, 0);
}

Panel_item extract_region_item;
Panel_item extract_set_item;
Panel_item extract_graph_item;

void do_extract_region(void)
{
    int gno, setno, regno;

    gno = (int) xv_get(extract_graph_item, PANEL_VALUE) - 1;
    if (gno == -1) {
	gno = cg;
    }
    setno = (int) xv_get(extract_set_item, PANEL_VALUE) - 1;
    if (setno == -1) {
	setno = nextset(gno);
	if (setno == -1) {
	    errwin("No more sets in this graph");
	    return;
	}
    }
    regno = (int) xv_get(extract_region_item, PANEL_VALUE);
    extract_region(gno, setno, regno);
    drawgraph();
}

void create_extract_frame(void)
{
    static Frame top;
    static Panel dialog;

    if (top) {
	xv_set(top, WIN_SHOW, TRUE, 0);
	return;
    }
    top = (Frame) xv_create(main_frame, FRAME,
			    FRAME_LABEL, "Extract region",
			    0);
    dialog = (Panel) xv_create(top, PANEL,
			       PANEL_LAYOUT, PANEL_VERTICAL,
			       0);

    extract_region_item = xv_create(dialog, PANEL_CYCLE,
				PANEL_LABEL_STRING, "Extract using region:",
				    PANEL_CHOICE_STRINGS,
		   "0", "1", "2", "3", "4", "Inside world", "Outside world",
				    0,
				    PANEL_VALUE_X, xv_col(dialog, 22),
				    0);

    define_select_set_panel4(dialog, extract_set_item, "To set:", 0, xv_row(dialog, 1));
    xv_set(extract_set_item, PANEL_VALUE_X, xv_col(dialog, 22), 0);
    extract_graph_item = (Panel_item) xv_create(dialog, PANEL_CHOICE_STACK,
					    PANEL_LABEL_STRING, "In graph:",
						PANEL_CHOICE_STRINGS,
						"Current",
			   "0", "1", "2", "3", "4", "5", "6", "7", "8", "9",
						0,
					  PANEL_VALUE_X, xv_col(dialog, 22),
						0);
    xv_create(dialog, PANEL_BUTTON,
	      PANEL_LABEL_STRING, "Apply",
	      PANEL_NOTIFY_PROC, do_extract_region,
	      XV_X, xv_col(dialog, 10),
	      XV_Y, xv_row(dialog, 4),
	      0);
    (void) xv_create(dialog, PANEL_BUTTON,
		     PANEL_LABEL_STRING, "Done",
		     PANEL_NOTIFY_PROC, generic_done_proc,
		     XV_KEY_DATA, FRAME_KEY, top,
		     XV_X, xv_col(dialog, 20),
		     XV_Y, xv_row(dialog, 4),
		     0);
    window_fit(dialog);
    window_fit(top);
    xv_set(top, WIN_SHOW, TRUE, 0);
}

Panel_item delete_region_item;
Panel_item delete_graph_item;

void do_delete_region(void)
{
    int gno, regno;

    gno = (int) xv_get(delete_graph_item, PANEL_VALUE) - 1;
    if (gno == -1) {
	gno = cg;
    }
    regno = (int) xv_get(delete_region_item, PANEL_VALUE);
    delete_region(gno, regno);
}

void create_delete_frame(void)
{
    static Frame top;
    static Panel dialog;

    if (top) {
	xv_set(top, WIN_SHOW, TRUE, 0);
	return;
    }
    top = (Frame) xv_create(main_frame, FRAME,
			    FRAME_LABEL, "Delete in region",
			    0);
    dialog = (Panel) xv_create(top, PANEL,
			       PANEL_LAYOUT, PANEL_VERTICAL,
			       0);

    delete_region_item = xv_create(dialog, PANEL_CYCLE,
				   PANEL_LABEL_STRING, "Delete in region:",
				   PANEL_CHOICE_STRINGS,
		   "0", "1", "2", "3", "4", "Inside world", "Outside world",
				   0,
				   PANEL_VALUE_X, xv_col(dialog, 18),
				   0);

    delete_graph_item = (Panel_item) xv_create(dialog, PANEL_CHOICE_STACK,
					    PANEL_LABEL_STRING, "In graph:",
					       PANEL_CHOICE_STRINGS,
					       "Current",
		    "0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "All",
					       0,
					  PANEL_VALUE_X, xv_col(dialog, 18),
					       0);
    xv_create(dialog, PANEL_BUTTON,
	      PANEL_LABEL_STRING, "Apply",
	      PANEL_NOTIFY_PROC, do_delete_region,
	      XV_X, xv_col(dialog, 5),
	      XV_Y, xv_row(dialog, 3),
	      0);
    (void) xv_create(dialog, PANEL_BUTTON,
		     PANEL_LABEL_STRING, "Done",
		     PANEL_NOTIFY_PROC, generic_done_proc,
		     XV_KEY_DATA, FRAME_KEY, top,
		     XV_X, xv_col(dialog, 15),
		     XV_Y, xv_row(dialog, 3),
		     0);
    window_fit(dialog);
    window_fit(top);
    xv_set(top, WIN_SHOW, TRUE, 0);
}

Panel_item arealab, perimlab;	/* loaded in events.c */

void do_select_area();
void do_select_peri();

void create_area_frame(void)
{
    static Frame top;
    static Panel dialog;

    if (top) {
	xv_set(top, WIN_SHOW, TRUE, 0);
	return;
    }
    top = (Frame) xv_create(main_frame, FRAME,
			    FRAME_LABEL, "Area/perimeter",
			    0);
    dialog = (Panel) xv_create(top, PANEL,
			       PANEL_LAYOUT, PANEL_VERTICAL,
			       0);

    arealab = xv_create(dialog, PANEL_TEXT,
			PANEL_LABEL_STRING, "Area = ",
			PANEL_VALUE_DISPLAY_LENGTH, 20,
			PANEL_VALUE_X, xv_col(dialog, 15),
			0);
    perimlab = xv_create(dialog, PANEL_TEXT,
			 PANEL_LABEL_STRING, "Perimeter = ",
			 PANEL_VALUE_DISPLAY_LENGTH, 20,
			 PANEL_VALUE_X, xv_col(dialog, 15),
			 0);

    (void) xv_create(dialog, PANEL_BUTTON,
		     PANEL_LABEL_STRING, "Area",
		     PANEL_NOTIFY_PROC, do_select_area,
		     XV_X, xv_col(dialog, 5),
		     XV_Y, xv_row(dialog, 3),
		     0);
    (void) xv_create(dialog, PANEL_BUTTON,
		     PANEL_LABEL_STRING, "Perimeter",
		     PANEL_NOTIFY_PROC, do_select_peri,
		     XV_X, xv_col(dialog, 15),
		     XV_Y, xv_row(dialog, 3),
		     0);
    (void) xv_create(dialog, PANEL_BUTTON,
		     PANEL_LABEL_STRING, "Done",
		     PANEL_NOTIFY_PROC, generic_done_proc,
		     XV_KEY_DATA, FRAME_KEY, top,
		     XV_X, xv_col(dialog, 27),
		     XV_Y, xv_row(dialog, 3),
		     0);
    window_fit(dialog);
    window_fit(top);
    xv_set(top, WIN_SHOW, TRUE, 0);
}

Panel_item eval_region;
Panel_item eval_region_item;

void do_eval_region(void)
{
    int regno;
    char buf[256];

    regno = (int) xv_get(eval_region, PANEL_VALUE);
    strcpy(buf, (char *) xv_get(eval_region_item, PANEL_VALUE));
    fixupstr(buf);
    evaluate_region(regno, buf);
}

void create_evalregion_frame(void)
{
    static Frame top;
    static Panel dialog;

    if (top) {
	xv_set(top, WIN_SHOW, TRUE, 0);
	return;
    }
    top = (Frame) xv_create(main_frame, FRAME,
			    FRAME_LABEL, "Evaluate in region",
			    0);
    dialog = (Panel) xv_create(top, PANEL,
			       PANEL_LAYOUT, PANEL_VERTICAL,
			       0);

    eval_region = xv_create(dialog, PANEL_CYCLE,
			    PANEL_LABEL_STRING, "Use region:",
			    PANEL_CHOICE_STRINGS,
			    "0", "1", "2", "3", "4", "Inside World", "Outside World",
			    0,
			    PANEL_VALUE_X, xv_col(dialog, 15),
			    0);

    eval_region_item = xv_create(dialog, PANEL_TEXT,
				 PANEL_LABEL_STRING, "Expression:",
				 PANEL_VALUE_DISPLAY_LENGTH, 25,
				 PANEL_VALUE_X, xv_col(dialog, 15),
				 0);

    xv_create(dialog, PANEL_BUTTON,
	      PANEL_LABEL_STRING, "Apply",
	      PANEL_NOTIFY_PROC, do_eval_region,
	      XV_X, xv_col(dialog, 5),
	      XV_Y, xv_row(dialog, 3),
	      0);
    (void) xv_create(dialog, PANEL_BUTTON,
		     PANEL_LABEL_STRING, "Done",
		     PANEL_NOTIFY_PROC, generic_done_proc,
		     XV_KEY_DATA, FRAME_KEY, top,
		     XV_X, xv_col(dialog, 15),
		     XV_Y, xv_row(dialog, 3),
		     0);
    window_fit(dialog);
    window_fit(top);
    xv_set(top, WIN_SHOW, TRUE, 0);
}
