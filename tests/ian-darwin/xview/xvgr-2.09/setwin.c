/* $Id: setwin.c,v 1.22 92/08/16 20:39:13 pturner Exp Locker: pturner $
 *
 * setops - operations on sets
 *
 */
#include <unistd.h>

#include <stdio.h>
#include <sys/param.h>
#include <math.h>

#include <xview/xview.h>
#include <xview/frame.h>
#include <xview/panel.h>

#include "globals.h"

char format[128] = "%lg %lg";	/* for write sets */
char sformat[128] = "%lg %lg";	/* for save all */

#define FRAME_KEY        101
void generic_done_proc();

extern Frame main_frame;

static char dirbuf[MAXPATHLEN];

static Panel_item activate_set_item;
static Panel_item alength_to_set_item;
static Panel_item set_length_item;
static Panel_item set_settype_item;
static Panel_item length_to_set_item;
static Panel_item deactivate_set_item;
static Panel_item reactivate_set_item;
static Panel_item copy_from_item;
static Panel_item copy_graph_item;
static Panel_item copy_to_item;
static Panel_item move_from_item;
static Panel_item move_graph_item;
static Panel_item move_to_item;
static Panel_item change_set_item;
static Panel_item change_settype_item;
static Panel_item swap_from_item;
static Panel_item swap_fgraph_item;
static Panel_item swap_to_item;
static Panel_item swap_tgraph_item;
static Panel_item drop_points_item;
static Panel_item drop_start_item;
static Panel_item drop_end_item;
static Panel_item join_from_item;
static Panel_item join_to_item;
static Panel_item part_set_item;
static Panel_item part_len_item;
static Panel_item kill_set_item;
static Panel_item kill_soft_toggle;
static Panel_item sort_set_item;
static Panel_item sort_xy_item;
static Panel_item sort_up_down_item;
static Panel_item write_sets_item;
static Panel_item write_setsgraph_item;
static Panel_item write_imbed_toggle;
static Panel_item write_pack_toggle;
static Panel_item write_sets_format_item;
static Panel_item write_sets_file_item;
Panel_item write_dir_msg_item;
static Panel_item saveall_sets_format_item;
static Panel_item saveall_sets_pack_item;
static Panel_item saveall_sets_file_item;
Panel_item saveall_dir_msg_item;
static Panel_item reverse_sets_item;
static Panel_item coalesce_sets_item;

static void do_setlength_proc();
static void do_changetype_proc();
static void do_activate_proc();
static void do_deactivate_proc();
static void do_reactivate_proc();
static void setops_done_proc();
static void do_copy_proc();
static void do_move_proc();
static void do_swap_proc();
static void do_drop_points_proc();
static void do_join_sets_proc();
static void do_split_sets_proc();
static void do_kill_proc();
void do_flush();
static void do_sort_proc();
static void do_reverse_sets_proc();
static void do_coalesce_sets_proc();
static Panel_setting do_write_sets_proc();
static Panel_setting do_saveall_sets_proc();

void create_activate_frame();
void create_copy_frame();
void create_change_frame();
void create_setlength_frame();
void create_move_frame();
void create_drop_frame();
void create_join_frame();
void create_split_frame();
void create_kill_frame();
void create_sort_frame();
void create_write_frame();
void create_reverse_frame();
void create_coalesce_frame();

static char errbuf[256];

extern int index_set_types[];
extern int index_set_ncols[];

void drawgraph();

int expand_tilde(char buf[]);
int isdir(char * f);
int my_chdir(char * buf);
void do_activate(int setno, int type, int len);
void do_changetype(int setno, int type);
void do_coalesce_sets(int setno);
void do_copy(int j1, int gfrom, int j2, int gto);
void do_deactivate(int gno, int setno);
void do_drop_points(int setno, int startno, int endno);
void do_join_sets(int gfrom, int j1, int gto, int j2);
void do_kill(int setno, int soft);
void do_move(int j1, int gfrom, int j2, int gto);
void do_reactivate(int gno, int setno);
void do_reverse_sets(int setno);
void do_setlength(int setno, int len);
void do_sort(int setno, int sorton, int stype);
void do_splitsets(int gno, int setno, int lpart);
void do_swap(int j1, int gfrom, int j2, int gto);
void do_writesets(int gno, int setno, int imbed, char * fn, char *format);
void errwin(char * msg);
void packsets(int gno);

void create_activate_frame(void)
{
    static Frame top;
    static Panel dialog;

    if (top) {
	xv_set(top, WIN_SHOW, TRUE, 0);
	return;
    }
    top = (Frame) xv_create(main_frame, FRAME,
			    FRAME_LABEL, "Activate sets",
			    0);
    dialog = (Panel) xv_create(top, PANEL,
			       PANEL_LAYOUT, PANEL_VERTICAL,
			       0);
    define_select_set_panel0(dialog, activate_set_item, "Activate set :", 20, 0);
    xv_set(activate_set_item, PANEL_VALUE_X, xv_col(dialog, 16), 0);
    alength_to_set_item = xv_create(dialog, PANEL_TEXT,
				    PANEL_VALUE_X, xv_col(dialog, 16),
				    PANEL_LABEL_STRING, "Length to set:",
				    PANEL_VALUE_DISPLAY_LENGTH, 10, 0);
    set_settype_item = (Panel_item) xv_create(dialog, PANEL_CHOICE_STACK,
					    PANEL_LABEL_STRING, "Set type:",
					      PANEL_CHOICE_STRINGS,
					      "XY",
					      "XY DX",
					      "XY DY",
					      "XY DX1 DX2",
					      "XY DY1 DY2",
					      "XY DX DY",
					      "XY Z",
					      "XY HILO",
					      0,
					  PANEL_VALUE_X, xv_col(dialog, 12),
					      0);
    xv_create(dialog, PANEL_BUTTON,
	      PANEL_LABEL_STRING, "Apply",
	      PANEL_NOTIFY_PROC, do_activate_proc,
	      XV_X, xv_col(dialog, 5),
	      XV_Y, xv_row(dialog, 3),
	      0);
    (void) xv_create(dialog, PANEL_BUTTON,
		     PANEL_LABEL_STRING, "Done",
		     PANEL_NOTIFY_PROC, generic_done_proc,
		     XV_X, xv_col(dialog, 15),
		     XV_Y, xv_row(dialog, 3),
		     XV_KEY_DATA, FRAME_KEY, top,
		     0);
    window_fit(dialog);
    window_fit(top);
    xv_set(top, WIN_SHOW, TRUE, 0);
}

void create_deactivate_frame(void)
{
    static Frame top;
    static Panel dialog;

    if (top) {
	xv_set(top, WIN_SHOW, TRUE, 0);
	return;
    }
    top = (Frame) xv_create(main_frame, FRAME,
			    FRAME_LABEL, "De-activate sets",
			    0);
    dialog = (Panel) xv_create(top, PANEL,
			       XV_HELP_DATA, "xvgr:deactivate_panel",
			       PANEL_LAYOUT, PANEL_VERTICAL,
			       0);
    define_select_set_panel0(dialog, deactivate_set_item, "De-activate set :", 20, 0);
    xv_set(deactivate_set_item, PANEL_VALUE_X, xv_col(dialog, 20), 0);
    xv_create(dialog, PANEL_BUTTON,
	      PANEL_LABEL_STRING, "Apply",
	      PANEL_NOTIFY_PROC, do_deactivate_proc,
	      XV_HELP_DATA, "xvgr:deactivate_apply",
	      XV_X, xv_col(dialog, 5),
	      XV_Y, xv_row(dialog, 1),
	      0);
    (void) xv_create(dialog, PANEL_BUTTON,
		     PANEL_LABEL_STRING, "Done",
		     PANEL_NOTIFY_PROC, generic_done_proc,
		     XV_X, xv_col(dialog, 15),
		     XV_Y, xv_row(dialog, 1),
		     XV_KEY_DATA, FRAME_KEY, top,
		     0);
    window_fit(dialog);
    window_fit(top);
    xv_set(top, WIN_SHOW, TRUE, 0);
}

void create_reactivate_frame(void)
{
    static Frame top;
    static Panel dialog;

    if (top) {
	xv_set(top, WIN_SHOW, TRUE, 0);
	return;
    }
    top = (Frame) xv_create(main_frame, FRAME,
			    FRAME_LABEL, "Re-activate sets",
			    0);
    dialog = (Panel) xv_create(top, PANEL,
			       XV_HELP_DATA, "xvgr:reactivate_panel",
			       PANEL_LAYOUT, PANEL_VERTICAL,
			       0);
    define_select_set_panel0(dialog, reactivate_set_item, "Re-activate set :", 20, 0);
    xv_set(reactivate_set_item, PANEL_VALUE_X, xv_col(dialog, 20), 0);
    xv_create(dialog, PANEL_BUTTON,
	      PANEL_LABEL_STRING, "Apply",
	      PANEL_NOTIFY_PROC, do_reactivate_proc,
	      XV_HELP_DATA, "xvgr:reactivate_apply",
	      XV_X, xv_col(dialog, 5),
	      XV_Y, xv_row(dialog, 1),
	      0);
    (void) xv_create(dialog, PANEL_BUTTON,
		     PANEL_LABEL_STRING, "Done",
		     PANEL_NOTIFY_PROC, generic_done_proc,
		     XV_X, xv_col(dialog, 15),
		     XV_Y, xv_row(dialog, 1),
		     XV_KEY_DATA, FRAME_KEY, top,
		     0);
    window_fit(dialog);
    window_fit(top);
    xv_set(top, WIN_SHOW, TRUE, 0);
}

void create_setlength_frame(void)
{
    static Frame top;
    static Panel dialog;

    if (top) {
	xv_set(top, WIN_SHOW, TRUE, 0);
	return;
    }
    top = (Frame) xv_create(main_frame, FRAME,
			    FRAME_LABEL, "Set length",
			    0);
    dialog = (Panel) xv_create(top, PANEL,
			       PANEL_LAYOUT, PANEL_VERTICAL,
			       0);
    define_select_set_panel0(dialog, set_length_item, "Set length of set: ", 10, 0);
    xv_set(set_length_item, PANEL_VALUE_X, xv_col(dialog, 20), 0);
    length_to_set_item = xv_create(dialog, PANEL_TEXT,
				   PANEL_VALUE_X, xv_col(dialog, 20),
				   PANEL_LABEL_STRING, "Length:",
				   PANEL_VALUE_DISPLAY_LENGTH, 10, 0);
    xv_create(dialog, PANEL_BUTTON,
	      PANEL_LABEL_STRING, "Apply",
	      PANEL_NOTIFY_PROC, do_setlength_proc,
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

void create_change_frame(void)
{
    static Frame top;
    static Panel dialog;

    if (top) {
	xv_set(top, WIN_SHOW, TRUE, 0);
	return;
    }
    top = (Frame) xv_create(main_frame, FRAME,
			    FRAME_LABEL, "Set type",
			    0);
    dialog = (Panel) xv_create(top, PANEL,
			       PANEL_LAYOUT, PANEL_VERTICAL,
			       0);
    define_select_set_panel0(dialog, change_set_item, "Set type of set:", 10, 0);
    xv_set(change_set_item, PANEL_VALUE_X, xv_col(dialog, 20), 0);
    change_settype_item = (Panel_item) xv_create(dialog, PANEL_CHOICE_STACK,
					    PANEL_LABEL_STRING, "Set type:",
						 PANEL_CHOICE_STRINGS,
						 "XY",
						 "XY DX",
						 "XY DY",
						 "XY DX1 DX2",
						 "XY DY1 DY2",
						 "XY DX DY",
						 "XY Z",
						 "XY HILO",
						 "XY RADIUS",
						 0,
					  PANEL_VALUE_X, xv_col(dialog, 20),
						 0);

    xv_create(dialog, PANEL_BUTTON,
	      PANEL_LABEL_STRING, "Apply",
	      PANEL_NOTIFY_PROC, do_changetype_proc,
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

void create_copy_frame(void)
{
    static Frame top;
    static Panel dialog;

    if (top) {
	xv_set(top, WIN_SHOW, TRUE, 0);
	return;
    }
    top = (Frame) xv_create(main_frame, FRAME,
			    FRAME_LABEL, "Copy set",
			    0);
    dialog = (Panel) xv_create(top, PANEL,
			       PANEL_LAYOUT, PANEL_VERTICAL,
			       0);

    define_select_set_panel0(dialog, copy_from_item, "Copy set:", 10, xv_row(dialog, 0));
    xv_set(copy_from_item, PANEL_VALUE_X, xv_col(dialog, 12), 0);
    define_select_set_panel4(dialog, copy_to_item, "To set:", 10, xv_row(dialog, 1));
    xv_set(copy_to_item, PANEL_VALUE_X, xv_col(dialog, 12), 0);
    copy_graph_item = (Panel_item) xv_create(dialog, PANEL_CHOICE_STACK,
					     PANEL_LABEL_STRING, "In graph:",
					     PANEL_CHOICE_STRINGS,
					     "Current",
			   "0", "1", "2", "3", "4", "5", "6", "7", "8", "9",
					     0,
					  PANEL_VALUE_X, xv_col(dialog, 12),
					     0);
    (void) xv_create(dialog, PANEL_BUTTON,
		     PANEL_LABEL_STRING, "Apply",
		     PANEL_NOTIFY_PROC, do_copy_proc,
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

void create_move_frame(void)
{
    static Frame top;
    static Panel dialog;

    if (top) {
	xv_set(top, WIN_SHOW, TRUE, 0);
	return;
    }
    top = (Frame) xv_create(main_frame, FRAME,
			    FRAME_LABEL, "Move set",
			    0);
    dialog = (Panel) xv_create(top, PANEL,
			       PANEL_LAYOUT, PANEL_VERTICAL,
			       0);

    define_select_set_panel0(dialog, move_from_item, "Move set:", 10, xv_row(dialog, 0));
    xv_set(move_from_item, PANEL_VALUE_X, xv_col(dialog, 12), 0);
    define_select_set_panel4(dialog, move_to_item, "To set:", 10, xv_row(dialog, 1));
    xv_set(move_to_item, PANEL_VALUE_X, xv_col(dialog, 12), 0);
    move_graph_item = (Panel_item) xv_create(dialog, PANEL_CHOICE_STACK,
					     PANEL_LABEL_STRING, "In graph:",
					     PANEL_CHOICE_STRINGS,
					     "Current",
			   "0", "1", "2", "3", "4", "5", "6", "7", "8", "9",
					     0,
					  PANEL_VALUE_X, xv_col(dialog, 12),
					     0);
    xv_create(dialog, PANEL_BUTTON,
	      PANEL_LABEL_STRING, "Apply",
	      PANEL_NOTIFY_PROC, do_move_proc,
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

void create_swap_frame(void)
{
    static Frame top;
    static Panel dialog;

    if (top) {
	xv_set(top, WIN_SHOW, TRUE, 0);
	return;
    }
    top = (Frame) xv_create(main_frame, FRAME,
			    FRAME_LABEL, "Swap sets",
			    0);
    dialog = (Panel) xv_create(top, PANEL,
			       PANEL_LAYOUT, PANEL_VERTICAL,
			       0);

    define_select_set_panel0(dialog, swap_from_item, "Swap set:", 10, xv_row(dialog, 0));
    xv_set(swap_from_item, PANEL_VALUE_X, xv_col(dialog, 12), 0);
    swap_fgraph_item = (Panel_item) xv_create(dialog, PANEL_CHOICE_STACK,
					    PANEL_LABEL_STRING, "In graph:",
					      PANEL_CHOICE_STRINGS,
					      "Current",
			   "0", "1", "2", "3", "4", "5", "6", "7", "8", "9",
					      0,
					  PANEL_VALUE_X, xv_col(dialog, 12),
					      0);
    define_select_set_panel0(dialog, swap_to_item, "With set:", 10, xv_row(dialog, 2));
    xv_set(swap_to_item, PANEL_VALUE_X, xv_col(dialog, 12), 0);
    swap_tgraph_item = (Panel_item) xv_create(dialog, PANEL_CHOICE_STACK,
					    PANEL_LABEL_STRING, "In graph:",
					      PANEL_CHOICE_STRINGS,
					      "Current",
			   "0", "1", "2", "3", "4", "5", "6", "7", "8", "9",
					      0,
					  PANEL_VALUE_X, xv_col(dialog, 12),
					      0);
    xv_create(dialog, PANEL_BUTTON,
	      PANEL_LABEL_STRING, "Apply",
	      PANEL_NOTIFY_PROC, do_swap_proc,
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

void create_drop_frame(void)
{
    static Frame top;
    static Panel dialog;

    if (top) {
	xv_set(top, WIN_SHOW, TRUE, 0);
	return;
    }
    top = (Frame) xv_create(main_frame, FRAME,
			    FRAME_LABEL, "Drop points",
			    0);
    dialog = (Panel) xv_create(top, PANEL,
			       PANEL_LAYOUT, PANEL_VERTICAL,
			       0);


    define_select_set_panel0(dialog, drop_points_item, "Drop points from set:", 10, 0);
    xv_set(drop_points_item, PANEL_VALUE_X, xv_col(dialog, 25), 0);
    drop_start_item = xv_create(dialog, PANEL_TEXT,
				PANEL_LABEL_STRING, "Start drop:",
				PANEL_VALUE_DISPLAY_LENGTH, 10,
				PANEL_VALUE_X, xv_col(dialog, 25),
				0);
    drop_end_item = xv_create(dialog, PANEL_TEXT,
			      PANEL_LABEL_STRING, "End drop:",
			      PANEL_VALUE_DISPLAY_LENGTH, 10,
			      PANEL_VALUE_X, xv_col(dialog, 25),
			      0);
    xv_create(dialog, PANEL_BUTTON,
	      PANEL_LABEL_STRING, "Apply",
	      PANEL_NOTIFY_PROC, do_drop_points_proc,
	      XV_X, xv_col(dialog, 10),
	      XV_Y, xv_row(dialog, 3),
	      0);
    (void) xv_create(dialog, PANEL_BUTTON,
		     PANEL_LABEL_STRING, "Done",
		     PANEL_NOTIFY_PROC, generic_done_proc,
		     XV_KEY_DATA, FRAME_KEY, top,
		     XV_X, xv_col(dialog, 20),
		     XV_Y, xv_row(dialog, 3),
		     0);
    window_fit(dialog);
    window_fit(top);
    xv_set(top, WIN_SHOW, TRUE, 0);
}

void create_join_frame(void)
{
    static Frame top;
    static Panel dialog;

    if (top) {
	xv_set(top, WIN_SHOW, TRUE, 0);
	return;
    }
    top = (Frame) xv_create(main_frame, FRAME,
			    FRAME_LABEL, "Join set",
			    0);
    dialog = (Panel) xv_create(top, PANEL,
			       PANEL_LAYOUT, PANEL_VERTICAL,
			       0);
    define_select_set_panel1(dialog, join_from_item, "Join set:", 10, xv_row(dialog, 0));
    xv_set(join_from_item, PANEL_VALUE_X, xv_col(dialog, 12), 0);
    define_select_set_panel0(dialog, join_to_item, "To set:", 10, xv_row(dialog, 1));
    xv_set(join_to_item, PANEL_VALUE_X, xv_col(dialog, 12), 0);
    (void) xv_create(dialog, PANEL_BUTTON,
		     PANEL_LABEL_STRING, "Apply",
		     PANEL_NOTIFY_PROC, do_join_sets_proc,
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

void create_split_frame(void)
{
    static Frame top;
    static Panel dialog;

    if (top) {
	xv_set(top, WIN_SHOW, TRUE, 0);
	return;
    }
    top = (Frame) xv_create(main_frame, FRAME,
			    FRAME_LABEL, "Split set",
			    0);
    dialog = (Panel) xv_create(top, PANEL,
			       PANEL_LAYOUT, PANEL_VERTICAL,
			       0);

    define_select_set_panel0(dialog, part_set_item, "Split set:", 10, 0);
    xv_set(part_set_item, PANEL_VALUE_X, xv_col(dialog, 12), 0);
    part_len_item = xv_create(dialog, PANEL_TEXT,
			      PANEL_LABEL_STRING, "Length:",
			      PANEL_VALUE_DISPLAY_LENGTH, 10,
			      PANEL_VALUE_X, xv_col(dialog, 12),
			      0);
    (void) xv_create(dialog, PANEL_BUTTON,
		     PANEL_LABEL_STRING, "Apply",
		     PANEL_NOTIFY_PROC, do_split_sets_proc,
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

void create_kill_frame(void)
{
    static Frame top;
    static Panel dialog;

    if (top) {
	xv_set(top, WIN_SHOW, TRUE, 0);
	return;
    }
    top = (Frame) xv_create(main_frame, FRAME,
			    FRAME_LABEL, "Kill set",
			    0);
    dialog = (Panel) xv_create(top, PANEL,
			       PANEL_LAYOUT, PANEL_VERTICAL,
			       0);

    define_select_set_panel1(dialog, kill_set_item, "Kill set:", 10, 0);
    kill_soft_toggle = (Panel_item) xv_create(dialog, PANEL_CHECK_BOX,
					      PANEL_LABEL_STRING,
					      "Preserve parameter settings:",
					      XV_X, xv_col(dialog, 0),
					      0);
    (void) xv_create(dialog, PANEL_BUTTON,
		     PANEL_LABEL_STRING, "Apply",
		     PANEL_NOTIFY_PROC, do_kill_proc,
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

void create_sort_frame(void)
{
    static Frame top;
    static Panel dialog;

    if (top) {
	xv_set(top, WIN_SHOW, TRUE, 0);
	return;
    }
    top = (Frame) xv_create(main_frame, FRAME,
			    FRAME_LABEL, "Sort set",
			    0);
    dialog = (Panel) xv_create(top, PANEL,
			       PANEL_LAYOUT, PANEL_VERTICAL,
			       0);

    define_select_set_panel0(dialog, sort_set_item, "Sort set:", 10, 0);
    xv_set(sort_set_item, PANEL_VALUE_X, xv_col(dialog, 15), 0);
    sort_xy_item = xv_create(dialog, PANEL_CYCLE,
			     PANEL_LABEL_STRING, "Sort on:",
			     PANEL_CHOICE_STRINGS,
			     "X",
			     "Y",
			     "*X then Y",
			     "*Y then X",
			     0,
			     PANEL_VALUE_X, xv_col(dialog, 15),
			     0);
    sort_up_down_item = xv_create(dialog, PANEL_CYCLE,
				  PANEL_LABEL_STRING, "Order:",
				  PANEL_CHOICE_STRINGS,
				  "Ascending",
				  "Descending", 0,
				  PANEL_VALUE_X, xv_col(dialog, 15),
				  0);
    (void) xv_create(dialog, PANEL_BUTTON,
		     PANEL_LABEL_STRING, "Apply",
		     PANEL_NOTIFY_PROC, do_sort_proc,
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

void create_write_frame(void)
{
    static Frame top;
    static Panel dialog;

    getcwd(dirbuf, MAXPATHLEN);
    if (top) {
	xv_set(write_dir_msg_item, PANEL_VALUE, dirbuf, 0);
	xv_set(top, WIN_SHOW, TRUE, 0);
	return;
    }
    top = (Frame) xv_create(main_frame, FRAME,
			    FRAME_LABEL, "Write set",
			    0);
    dialog = (Panel) xv_create(top, PANEL,
			       PANEL_LAYOUT, PANEL_VERTICAL,
			       0);
    write_dir_msg_item = (Panel_item) xv_create(dialog, PANEL_MESSAGE,
						PANEL_LABEL_STRING, dirbuf,
						0);
    define_select_set_panel1(dialog, write_sets_item, "Write set:", 0, xv_row(dialog, 1));
    xv_set(write_sets_item, PANEL_VALUE_X, xv_col(dialog, 18), 0);

    write_imbed_toggle = (Panel_item) xv_create(dialog, PANEL_CHECK_BOX,
						PANEL_CHOICE_STRINGS,
						"Imbed parameters",
						0,
					  PANEL_VALUE_X, xv_col(dialog, 18),
						0);
    write_pack_toggle = (Panel_item) xv_create(dialog, PANEL_CHECK_BOX,
					       PANEL_CHOICE_STRINGS,
					       "Pack sets",
					       0,
					  PANEL_VALUE_X, xv_col(dialog, 18),
					       0);
    write_setsgraph_item = (Panel_item) xv_create(dialog, PANEL_CYCLE,
					  PANEL_LABEL_STRING, "From graph:",
						  PANEL_CHOICE_STRINGS,
					 "Current", "0", "1", "2", "3", "4",
						  "5", "6", "7", "8", "9",
						  "All active graphs",
						  0,
					  PANEL_VALUE_X, xv_col(dialog, 18),
						  0);
    write_sets_format_item = xv_create(dialog, PANEL_TEXT,
				       PANEL_LABEL_STRING, "Format:",
				       PANEL_VALUE_X, xv_col(dialog, 18),
				       PANEL_VALUE_DISPLAY_LENGTH, 10, 0);
    write_sets_file_item = xv_create(dialog, PANEL_TEXT,
				     PANEL_NOTIFY_PROC, do_write_sets_proc,
				     PANEL_LABEL_STRING, "Write to file:",
				     PANEL_VALUE_X, xv_col(dialog, 18),
				     PANEL_VALUE_DISPLAY_LENGTH, 20, 0);
    xv_create(dialog, PANEL_BUTTON,
	      PANEL_LABEL_STRING, "Apply",
	      PANEL_NOTIFY_PROC, do_write_sets_proc,
	      XV_X, xv_col(dialog, 5),
	      XV_Y, xv_row(dialog, 9),
	      0);
    (void) xv_create(dialog, PANEL_BUTTON,
		     PANEL_LABEL_STRING, "Done",
		     PANEL_NOTIFY_PROC, generic_done_proc,
		     XV_KEY_DATA, FRAME_KEY, top,
		     XV_X, xv_col(dialog, 15),
		     XV_Y, xv_row(dialog, 9),
		     0);
    window_fit(dialog);
    window_fit(top);
    xv_set(write_sets_format_item, PANEL_VALUE, format, 0);
    xv_set(top, WIN_SHOW, TRUE, 0);
}

void create_reverse_frame(void)
{
    static Frame top;
    static Panel dialog;

    if (top) {
	xv_set(top, WIN_SHOW, TRUE, 0);
	return;
    }
    top = (Frame) xv_create(main_frame, FRAME,
			    FRAME_LABEL, "Reverse set",
			    0);
    dialog = (Panel) xv_create(top, PANEL,
			       PANEL_LAYOUT, PANEL_VERTICAL,
			       0);

    define_select_set_panel0(dialog, reverse_sets_item, "Reverse set:", 10, 0);
    (void) xv_create(dialog, PANEL_BUTTON,
		     PANEL_LABEL_STRING, "Apply",
		     PANEL_NOTIFY_PROC, do_reverse_sets_proc,
		     XV_X, xv_col(dialog, 5),
		     XV_Y, xv_row(dialog, 1),
		     0);
    (void) xv_create(dialog, PANEL_BUTTON,
		     PANEL_LABEL_STRING, "Done",
		     PANEL_NOTIFY_PROC, generic_done_proc,
		     XV_KEY_DATA, FRAME_KEY, top,
		     XV_X, xv_col(dialog, 15),
		     XV_Y, xv_row(dialog, 1),
		     0);
    window_fit(dialog);
    window_fit(top);
    xv_set(top, WIN_SHOW, TRUE, 0);
}

void create_coalesce_frame(void)
{
    static Frame top;
    static Panel dialog;

    if (top) {
	xv_set(top, WIN_SHOW, TRUE, 0);
	return;
    }
    top = (Frame) xv_create(main_frame, FRAME,
			    FRAME_LABEL, "Coalesce sets",
			    0);
    dialog = (Panel) xv_create(top, PANEL,
			       PANEL_LAYOUT, PANEL_VERTICAL,
			       0);

    define_select_set_panel0(dialog, coalesce_sets_item, "Coalesce all active sets to set:", 10, 0);
    (void) xv_create(dialog, PANEL_BUTTON,
		     PANEL_LABEL_STRING, "Apply",
		     PANEL_NOTIFY_PROC, do_coalesce_sets_proc,
		     XV_X, xv_col(dialog, 5),
		     XV_Y, xv_row(dialog, 1),
		     0);
    (void) xv_create(dialog, PANEL_BUTTON,
		     PANEL_LABEL_STRING, "Done",
		     PANEL_NOTIFY_PROC, generic_done_proc,
		     XV_KEY_DATA, FRAME_KEY, top,
		     XV_X, xv_col(dialog, 15),
		     XV_Y, xv_row(dialog, 1),
		     0);
    window_fit(dialog);
    window_fit(top);
    xv_set(top, WIN_SHOW, TRUE, 0);
}

/*
 * activate a set and set its length
 */
static void do_activate_proc(void)
{
    int setno, len, type;

    setno = (int) xv_get(activate_set_item, PANEL_VALUE);
    type = (int) xv_get(set_settype_item, PANEL_VALUE);
    len = atoi((char *) xv_get(alength_to_set_item, PANEL_VALUE));
    do_activate(setno, type, len);
}

/*
 * de-activate a set
 */
static void do_deactivate_proc(void)
{
    int setno;

    setno = (int) xv_get(deactivate_set_item, PANEL_VALUE);
    do_deactivate(cg, setno);
}

/*
 * re-activate a set
 */
static void do_reactivate_proc(void)
{
    int setno;

    setno = (int) xv_get(reactivate_set_item, PANEL_VALUE);
    do_reactivate(cg, setno);
}

/*
 * set the length of an active set - contents are destroyed
 */
static void do_setlength_proc(void)
{
    int setno, len;

    setno = (int) xv_get(set_length_item, PANEL_VALUE);
    len = atoi((char *) xv_get(length_to_set_item, PANEL_VALUE));
    do_setlength(setno, len);
}

/*
 * change the type of a set
 */
static void do_changetype_proc(void)
{
    int setno, type;

    setno = (int) xv_get(change_set_item, PANEL_VALUE);
    type = (int) xv_get(change_settype_item, PANEL_VALUE);
    do_changetype(setno, type);
}

/*
 * copy a set to another set, if the to set doesn't exist
 * get a new one, if it does, ask if it is okay to overwrite
 */
static void do_copy_proc(void)
{
    int j1, j2, gto;
    char buf[32];

    j1 = (int) xv_get(copy_from_item, PANEL_VALUE);
    j2 = (int) xv_get(copy_to_item, PANEL_VALUE);
    gto = (int) xv_get(copy_graph_item, PANEL_VALUE);
    do_copy(j1, cg, j2, gto);
}

/*
 * move a set to another set, if the to set doesn't exist
 * get a new one, if it does, ask if it is okay to overwrite
 */
static void do_move_proc(void)
{
    int j1, j2, gto;

    j1 = (int) xv_get(move_from_item, PANEL_VALUE);
    gto = (int) xv_get(move_graph_item, PANEL_VALUE);
    j2 = (int) xv_get(move_to_item, PANEL_VALUE) - 1;
    do_move(j1, cg, j2, gto);
}

/*
 * swap a set with another set
 */
static void do_swap_proc(void)
{
    int j1, j2, gto, gfrom;

    j1 = (int) xv_get(swap_from_item, PANEL_VALUE);
    gfrom = (int) xv_get(swap_fgraph_item, PANEL_VALUE);
    j2 = (int) xv_get(swap_to_item, PANEL_VALUE);
    gto = (int) xv_get(swap_tgraph_item, PANEL_VALUE);
    do_swap(j1, gfrom, j2, gto);
}

/*
 * drop points from an active set
 */
static void do_drop_points_proc(void)
{
    int startno, endno, setno;

    setno = (int) xv_get(drop_points_item, PANEL_VALUE);
    startno = atoi((char *) xv_get(drop_start_item, PANEL_VALUE)) - 1;
    endno = atoi((char *) xv_get(drop_end_item, PANEL_VALUE)) - 1;
    do_drop_points(setno, startno, endno);
}

/*
 * append one set to another
 */
static void do_join_sets_proc(void)
{
    int j1, j2, i;

    j1 = (int) xv_get(join_from_item, PANEL_VALUE);
    j2 = (int) xv_get(join_to_item, PANEL_VALUE);
    if (j1 == MAXPLOT) {
	j1 = -1;
    }
    do_join_sets(cg, j1, cg, j2);
}

/*
 * reverse the order of a set
 */
static void do_reverse_sets_proc(void)
{
    int setno;

    setno = (int) xv_get(reverse_sets_item, PANEL_VALUE);
    do_reverse_sets(setno);
}

/*
 * coalesce sets
 */
static void do_coalesce_sets_proc(void)
{
    int setno;

    setno = (int) xv_get(coalesce_sets_item, PANEL_VALUE);
    do_coalesce_sets(setno);
}

/*
 * kill a set
 */
static void do_kill_proc(void)
{
    int setno, soft, redraw = 0, i;

    setno = (int) xv_get(kill_set_item, PANEL_VALUE);
    soft = (int) xv_get(kill_soft_toggle, PANEL_VALUE);
    do_kill(setno, soft);
}

/*
 sort sets
*/
static void do_sort_proc(void)
{
    int setno, sorton, stype, i;

    setno = (int) xv_get(sort_set_item, PANEL_VALUE);
    if (setno == MAXPLOT) {
	setno = -1;
    }
    sorton = (int) xv_get(sort_xy_item, PANEL_VALUE);
    stype = (int) xv_get(sort_up_down_item, PANEL_VALUE);
    do_sort(setno, sorton, stype);
}

/*
 *  write a set or sets to a file
 */
static Panel_setting do_write_sets_proc(Panel_item item, Event * event)
{
    int which_graph;
    char fn[MAXPATHLEN];
    int setno;
    int imbed, pack, i;

    imbed = (int) xv_get(write_imbed_toggle, PANEL_VALUE);
    setno = (int) xv_get(write_sets_item, PANEL_VALUE);
    if (setno == MAXPLOT) {
	setno = -1;
    }
    pack = (int) xv_get(write_imbed_toggle, PANEL_VALUE);
    which_graph = (int) xv_get(write_setsgraph_item, PANEL_VALUE) - 1;
    if (pack) {
	if (which_graph == MAXGRAPH) {
	    for (i = 0; i < maxgraph; i++) {
		packsets(i);
	    }
	} else if (which_graph == -1) {
	    packsets(cg);
	} else {
	    packsets(which_graph);
	}
    }
    strcpy(fn, (char *) xv_get(write_sets_file_item, PANEL_VALUE));
    strcpy(format, (char *) xv_get(write_sets_format_item, PANEL_VALUE));
    if (fn[0] == '~') {
	expand_tilde(fn);
    }
    if (isdir(fn)) {
	if (my_chdir(fn)) {
	    errwin("Can't change directory");
	}
    } else {
	do_writesets(which_graph, setno, imbed, fn, format);
    }
    return PANEL_NONE;
}

/*
 * split sets split by itmp, remainder in last set.
 */
static void do_split_sets_proc(void)
{
    int setno, lpart;

    setno = (int) xv_get(part_set_item, PANEL_VALUE);
    lpart = atoi((char *) xv_get(part_len_item, PANEL_VALUE));
    do_splitsets(cg, setno, lpart);
}

void create_saveall_frame(void)
{
    static Frame top;
    static Panel dialog;

    getcwd(dirbuf, MAXPATHLEN);
    if (top) {
	xv_set(saveall_dir_msg_item, PANEL_VALUE, dirbuf, 0);
	xv_set(top, WIN_SHOW, TRUE, 0);
	return;
    }
    top = (Frame) xv_create(main_frame, FRAME,
			    FRAME_LABEL, "Save all",
			    0);
    dialog = (Panel) xv_create(top, PANEL,
			       PANEL_LAYOUT, PANEL_VERTICAL,
			       0);
    saveall_dir_msg_item = (Panel_item) xv_create(dialog, PANEL_MESSAGE,
						  PANEL_LABEL_STRING, dirbuf,
						  0);

    saveall_sets_format_item = xv_create(dialog, PANEL_TEXT,
					 PANEL_LABEL_STRING, "Format:",
					 PANEL_VALUE_X, xv_col(dialog, 18),
				      PANEL_VALUE_DISPLAY_LENGTH, 10, 0);
    saveall_sets_pack_item = (Panel_item) xv_create(dialog, PANEL_CHECK_BOX,
						    PANEL_CHOICE_STRINGS,
						    "Pack sets",
						    0,
					  PANEL_VALUE_X, xv_col(dialog, 18),
						    0);
    saveall_sets_file_item = xv_create(dialog, PANEL_TEXT,
				    PANEL_NOTIFY_PROC, do_saveall_sets_proc,
				       PANEL_LABEL_STRING, "Write to file:",
				       PANEL_VALUE_X, xv_col(dialog, 18),
				       PANEL_VALUE_DISPLAY_LENGTH, 20, 0);
    xv_create(dialog, PANEL_BUTTON,
	      PANEL_LABEL_STRING, "Apply",
	      PANEL_NOTIFY_PROC, do_saveall_sets_proc,
	      XV_X, xv_col(dialog, 5),
	      XV_Y, xv_row(dialog, 5),
	      0);
    (void) xv_create(dialog, PANEL_BUTTON,
		     PANEL_LABEL_STRING, "Done",
		     PANEL_NOTIFY_PROC, generic_done_proc,
		     XV_KEY_DATA, FRAME_KEY, top,
		     XV_X, xv_col(dialog, 15),
		     XV_Y, xv_row(dialog, 5),
		     0);
    window_fit(dialog);
    window_fit(top);
    xv_set(saveall_sets_format_item, PANEL_VALUE, sformat, 0);
    xv_set(top, WIN_SHOW, TRUE, 0);
}

/*
 *  write a set or sets to a file
 */
static Panel_setting do_saveall_sets_proc(Panel_item item, Event * event)
{
    int which_graph;
    int setno;
    int imbed, pack, i;
    char fn[MAXPATHLEN];

    imbed = 1;
    setno = -1;
    which_graph = MAXGRAPH;
    pack = (int) xv_get(saveall_sets_pack_item, PANEL_VALUE);
    if (pack) {
	if (which_graph == MAXGRAPH) {
	    for (i = 0; i < maxgraph; i++) {
		packsets(i);
	    }
	} else {
	    packsets(which_graph);
	}
    }
    strcpy(fn, (char *) xv_get(saveall_sets_file_item, PANEL_VALUE));
    if (fn[0] == '~') {
	expand_tilde(fn);
    }
    if (isdir(fn)) {
	if (my_chdir(fn)) {
	    errwin("Can't change directory");
	}
    } else {
	strcpy(sformat, (char *) xv_get(saveall_sets_format_item, PANEL_VALUE));
	do_writesets(which_graph, setno, imbed, fn, sformat);
    }
    return PANEL_NONE;
}
