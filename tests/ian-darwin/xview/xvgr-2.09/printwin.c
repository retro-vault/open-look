/* $Id: printwin.c,v 1.11 92/07/19 08:13:47 pturner Exp Locker: pturner $
 *
 * Printer initialization
 */

#include <stdio.h>
#include <xview/xview.h>
#include <xview/frame.h>
#include <xview/canvas.h>
#include <xview/panel.h>
#include <xview/font.h>
#include "globals.h"

extern Frame main_frame;	/* defined in xvgr.c */

void do_hardcopy();

static Panel_item printto_item;	/* for printer select popup */
static Panel_item printstring_item;
static void update_printer_setup();
static void set_printer_proc();
static void printer_done_proc();
static void do_pr_toggle();
static void do_prstr_toggle();
static void do_print();

Frame psetup_frame;
Panel psetup_panel;
Panel_item devices_item;

char noprint[] = "No printer installed";

/*
 * set the current print options
 */
void errwin(char * msg);

void set_printer(int device, char * prstr)
{
    if (device == FILEP) {
	if (prstr != 0) {
	    strcpy(printstr, prstr);
	}
	ptofile = TRUE;
    } else {
	switch (device) {
	case GR_PS_L:
	case GR_PS_P:
	    if (prstr != 0) {
		strcpy(ps_prstr, prstr);
	    }
	    curprint = ps_prstr;
	    break;
	case GR_MIF_L:
	case GR_MIF_P:
	    if (prstr != 0) {
		strcpy(mif_prstr, prstr);
	    }
	    curprint = mif_prstr;
	    break;
	case GR_HPGL_L:
	case GR_HPGL_P:
	    if (prstr != 0) {
		strcpy(hp_prstr1, prstr);
	    }
	    curprint = hp_prstr1;
	    break;
	case GR_LEAF_L:
	case GR_LEAF_P:
	    if (prstr != 0) {
		strcpy(leaf_prstr, prstr);
	    }
	    curprint = leaf_prstr;
	    break;
	default:
	    sprintf(buf, "Unknown printer device %d, printer unchanged", device);
	    errwin(buf);
	    return;
	    break;
	}
	hdevice = device;
	ptofile = FALSE;
    }
    if (psetup_frame) {
	update_printer_setup();
    }
}

/*
 * make the printer popup
 */
void create_printer_setup(void)
{
    if (psetup_frame) {
	update_printer_setup();
	xv_set(psetup_frame, WIN_SHOW, TRUE, 0);
	return;
    }
    psetup_frame = xv_create(main_frame, FRAME,
			     WIN_Y, 50,
			     WIN_X, 50,
			     FRAME_LABEL, "Printer select",
			     FRAME_SHOW_LABEL, TRUE,
			     WIN_ERROR_MSG, "Couldn't create psetup_frame",
			     0);
    psetup_panel = xv_create(psetup_frame, PANEL,
			     PANEL_LAYOUT, PANEL_VERTICAL,
			     0);
    devices_item = xv_create(psetup_panel, PANEL_CYCLE,
			     PANEL_LABEL_STRING, "Device:",
			     PANEL_VALUE_X, xv_col(psetup_panel, 25),
			     PANEL_NOTIFY_PROC,
			     do_prstr_toggle,
			     PANEL_CHOICE_STRINGS,
			     "PostScript landscape",
			     "PostScript portrait",
			     "FrameMaker landscape",
			     "FrameMaker portrait",
			     "HPGL landscape",
			     "HPGL portrait",
			     "Interleaf landscape",
			     "Interleaf portrait",
			     0, 0);
    printto_item = xv_create(psetup_panel, PANEL_CYCLE,
			     PANEL_LABEL_STRING, "Print to:",
			     PANEL_VALUE_X, xv_col(psetup_panel, 25),
			     PANEL_NOTIFY_PROC,
			     do_pr_toggle,
			     PANEL_CHOICE_STRINGS,
			     "Printer",
			     "File", 0, 0);
    printstring_item = xv_create(psetup_panel, PANEL_TEXT,
			      PANEL_LABEL_STRING, "Printer control string:",
				 PANEL_VALUE_X, xv_col(psetup_panel, 25),
				 PANEL_VALUE_DISPLAY_LENGTH, 30, 0);
    xv_create(psetup_panel, PANEL_BUTTON,
	      XV_X, xv_col(psetup_panel, 30),
	      XV_Y, xv_row(psetup_panel, 4),
	      PANEL_LABEL_STRING, "Done",
	      PANEL_NOTIFY_PROC, printer_done_proc, 0);
    xv_create(psetup_panel, PANEL_BUTTON,
	      XV_X, xv_col(psetup_panel, 20),
	      XV_Y, xv_row(psetup_panel, 4),
	      PANEL_LABEL_STRING, "Print",
	      PANEL_NOTIFY_PROC, do_print, 0);
    xv_create(psetup_panel, PANEL_BUTTON,
	      XV_X, xv_col(psetup_panel, 10),
	      XV_Y, xv_row(psetup_panel, 4),
	      PANEL_LABEL_STRING, "Accept",
	      PANEL_NOTIFY_PROC, set_printer_proc, 0);
    update_printer_setup();
    window_fit(psetup_panel);
    window_fit(psetup_frame);
    xv_set(psetup_frame, WIN_SHOW, TRUE, 0);
}

static void printer_done_proc(void)
{
    xv_set(psetup_frame, WIN_SHOW, FALSE, 0);
}

static void update_printer_setup(void)
{
    xv_set(devices_item, PANEL_VALUE, hdevice - 1, 0);
    xv_set(printto_item, PANEL_VALUE, ptofile, 0);
    if (ptofile) {
	xv_set(printstring_item, PANEL_VALUE, printstr, 0);
    } else {
	xv_set(printstring_item, PANEL_VALUE, curprint, 0);
    }
}

/*
 * if Accept'ed
 */
static void set_printer_proc(void)
{
    char tmpstr[128];

    hdevice = (int) xv_get(devices_item, PANEL_VALUE) + 1;
    ptofile = (int) xv_get(printto_item, PANEL_VALUE);
    strcpy(tmpstr, (char *) xv_get(printstring_item, PANEL_VALUE));
    if (ptofile) {
	strcpy(printstr, tmpstr);
    } else {
	strcpy(curprint, tmpstr);
    }
}

/*
 * Print button
 */
static void do_print(void)
{
    set_printer_proc();
    do_hardcopy();
}

/*
 * set the print options
 */
/* ARGSUSED */
static void do_prstr_toggle(Panel_item item, int value, Event * event)
{
    set_printer(value + 1, 0);
    if ((int) xv_get(printto_item, PANEL_VALUE) == 0) {
	xv_set(printstring_item, PANEL_VALUE, curprint, 0);
    }
}

/*
 * toggle between a file and the printer
 */
/* ARGSUSED */
static void do_pr_toggle(Panel_item item, int value, Event * event)
{
    if (value) {
	xv_set(printstring_item, PANEL_LABEL_STRING, "Print to file:", 0);
	xv_set(printstring_item, PANEL_VALUE, printstr, 0);
    } else {
	xv_set(printstring_item, PANEL_LABEL_STRING, "Printer control string:", 0);
	xv_set(printstring_item, PANEL_VALUE, curprint, 0);
    }
}
