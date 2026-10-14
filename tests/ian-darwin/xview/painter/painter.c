/*
 * painter.c -- a short example using the XView/NeWS library
 * PostScript Code and ideas taken from Lavallee's painter script.
 * TODO - save a point list, and redraw it in the event proc.
 */

#include <xview/xview.h>
#include <xview/canvas.h>
#include <xview/panel.h>
#include <xvps/pscanvas.h>
#include "painter.h"

void
pscanvas_repaint_proc(PScanvas pscanvas, NeWStoken newstoken, Display	* dpy, Window xid, Xv_xrectlist * xrects)
{
	xv_set(pscanvas, PSCANVAS_CLIPRECTS, xrects, 0);
	/* pscanvas_flip(); */
	painter_repaint();
}

int main(int argc, char ** argv)
{
	Frame frame;
	Menu menu;
	Panel panel;
	void menu_proc();
	int menu_hit();
	Canvas pcan;

	xv_init(XV_INIT_ARGS, argc, argv, 0);

	/* Make a baseframe */
	frame=xv_create(0, FRAME,
		FRAME_LABEL, "Toy XVPS Paint Program (use snapshot to save)",
		0);

	/* Panel is just to hold buttons */
	panel = (Panel) xv_create(frame, PANEL,
		XV_HEIGHT, 80,
		0);

	menu = (Menu) xv_create(0, MENU,
		MENU_NOTIFY_PROC, menu_proc,
		MENU_STRINGS,		"Clear", "Save...", 0,
		0);
	(void) xv_create(panel, PANEL_BUTTON,
		PANEL_LABEL_STRING,	"Edit",
		PANEL_NOTIFY_PROC,	menu_hit,
		PANEL_MENU_ITEM,	menu,
		0);

	/* And finally a PSCANVAS to scribble on */
	pcan = (Canvas) xv_create(frame, PSCANVAS,
		PSCANVAS_REPAINT_PROC, pscanvas_repaint_proc,
		PSCANVAS_SYNC, 5,
		0);

	/* Now call some procs that send stuff */
	DEFINE_paintit();
	DEFINE_setxy();

	/* Now call a CPS proc that loads our main program */
	painter_main();

	window_fit(pcan);
	window_fit(frame);

	/* And finally the canonical event loop... */
	xv_main_loop(frame);
}

int
menu_hit(void)
{
	return XV_OK;
}

void
menu_proc(Menu menu, Menu_item item)
{
	fprintf(stderr, "Menu item %s\n", xv_get(item, MENU_STRING));

	/* TODO actions... */
}
