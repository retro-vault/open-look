/* $Id: propswin.c,v 1.2 92/08/01 08:31:32 pturner Exp Locker: pturner $
 *
 * frame Panel
 *
 */

#include <stdio.h>
#include <xview/xview.h>
#include <xview/panel.h>

#include "globals.h"

Frame props_frame = (Frame) 0;
Panel props_panel;

/*
 * Panel item declarations
 */
static Panel_item debug_item;
static Panel_item invert_item;
static Panel_item bs_item;
static Panel_item refresh_item;
static Panel_item rvideo_item;
static Panel_item maxplot_item;
static Panel_item maxgraph_item;
static Panel_item maxcolors_item;
static Panel_item auto_item;
static Panel_item verify_item;
static Panel_item dc_item;

/*
 * Event and Notify proc declarations
 */
static int props_Done_notify_proc();
static int props_define_notify_proc();

void update_props_items(void)
{
    if (props_frame) {
	xv_set(debug_item, PANEL_VALUE, debuglevel, 0);
/*
 *  xv_set(invert_item, PANEL_VALUE, invert, 0);
 *  xv_set(bs_item, PANEL_VALUE, backingstore, 0);
 *  xv_set(refresh_item, PANEL_VALUE, allow_refresh, 0);
 *  xv_set(rvideo_item, PANEL_VALUE, revflag, 0);
 *  xv_set(maxplot_item, PANEL_VALUE, maxplot, 0);
 *  xv_set(maxgraph_item, PANEL_VALUE, maxgraph, 0);
 *  xv_set(maxcolors_item, PANEL_VALUE, maxcolors, 0);
 */
	xv_set(verify_item, PANEL_VALUE, verify_action, 0);
	xv_set(dc_item, PANEL_VALUE, allow_dc, 0);
	xv_set(auto_item, PANEL_VALUE, autoscale_onread, 0);
    }
}

/*
 * Create the page Frame and the page Panel
 */
void create_props_frame(void)
{
    extern Frame main_frame;

    if (props_frame) {
	update_props_items();
	xv_set(props_frame, WIN_SHOW, TRUE, 0);
	return;
    }
    props_frame = (Frame) xv_create(main_frame, FRAME,
				    FRAME_LABEL, "Misc. properties",
				    0);
    props_panel = (Panel) xv_create(props_frame, PANEL,
				    XV_HELP_DATA, "xvgr:props_panel",
				    PANEL_LAYOUT, PANEL_VERTICAL,
				    0);
    debug_item = (Panel_item) xv_create(props_panel, PANEL_CHOICE_STACK,
					XV_HELP_DATA, "xvgr:props_debug",
					PANEL_LABEL_STRING, "Debug level:",
					PANEL_CHOICE_STRINGS,
					"Off",
					"1",
					"2",
					"3",
					"4",
					"5",
					"6",
					"7",
					"8",
					0,
				     PANEL_VALUE_X, xv_col(props_panel, 32),
					0);
    verify_item = (Panel_item) xv_create(props_panel, PANEL_CHECK_BOX,
					 XV_HELP_DATA, "xvgr:props_verify",
					 PANEL_LABEL_STRING,
					 "Verify Pick sets operations:",
				     PANEL_VALUE_X, xv_col(props_panel, 32),
					 0);
    dc_item = (Panel_item) xv_create(props_panel, PANEL_CHECK_BOX,
				     XV_HELP_DATA, "xvgr:props_dc",
				     PANEL_LABEL_STRING,
				     "Allow double clicks on canvas:",
				     PANEL_VALUE_X, xv_col(props_panel, 32),
				     0);
    auto_item = (Panel_item) xv_create(props_panel, PANEL_CHECK_BOX,
				       XV_HELP_DATA, "xvgr:props_auto",
				       PANEL_LABEL_STRING,
				       "Allow autoscale on read:",
				     PANEL_VALUE_X, xv_col(props_panel, 32),
				       0);
    (void) xv_create(props_panel, PANEL_BUTTON,
		     XV_HELP_DATA, "xvgr:props_done",
		     PANEL_LABEL_STRING, "Done",
		     PANEL_NOTIFY_PROC, props_Done_notify_proc,
		     XV_X, xv_col(props_panel, 12),
		     XV_Y, xv_row(props_panel, 6),
		     0);
    (void) xv_create(props_panel, PANEL_BUTTON,
		     XV_HELP_DATA, "xvgr:props_accept",
		     PANEL_LABEL_STRING, "Accept",
		     PANEL_NOTIFY_PROC, props_define_notify_proc,
		     XV_X, xv_col(props_panel, 1),
		     XV_Y, xv_row(props_panel, 6),
		     0);
    window_fit(props_panel);
    window_fit(props_frame);
    update_props_items();
    xv_set(props_frame, WIN_SHOW, TRUE, 0);
}				/* end create_props_panel */

/*
 * Notify and event procs
 */

/*ARGSUSED*/
static int props_Done_notify_proc(Panel_item item, Event * event)
{
    xv_set(props_frame, WIN_SHOW, FALSE, 0);
}

/*ARGSUSED*/
static int props_define_notify_proc(Panel_item item, Event * event)
{
    int value;

    debuglevel = (int) xv_get(debug_item, PANEL_VALUE);
/*
 *  invert = (int) xv_get(invert_item, PANEL_VALUE);
 *  backingstore = (int) xv_get(bs_item, PANEL_VALUE);
 *  allow_refresh = (int) xv_get(refresh_item, PANEL_VALUE);
 *  revflag = (int) xv_get(rvideo_item, PANEL_VALUE);
 *  maxplot = (int) xv_get(maxplot_item, PANEL_VALUE);
 *  maxgraph = (int) xv_get(maxgraph_item, PANEL_VALUE);
 *  maxcolors = (int) xv_get(maxcolors_item, PANEL_VALUE);
*/
    verify_action = (int) xv_get(verify_item, PANEL_VALUE);
    allow_dc = (int) xv_get(dc_item, PANEL_VALUE);
    autoscale_onread = (int) xv_get(auto_item, PANEL_VALUE);
    return XV_OK;
}
