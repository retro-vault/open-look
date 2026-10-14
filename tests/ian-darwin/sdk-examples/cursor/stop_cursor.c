/*
 * color_cursor.c -- create a predefined cursor and assign it to a canvas.
 */
#include <stdio.h>
#include <xview/xview.h>
#include <xview/panel.h>
#include <xview/cursor.h>

void
do_it(Panel_item item, Event *event)
{
    Rect *r;
    Panel panel = xv_get(item, PANEL_PARENT_PANEL);

    r = (Rect *)xv_get(xv_get(panel, XV_ROOT), WIN_MOUSE_XY);
    fprintf(stderr, "Root window: ");
    rect_print(r);
    fputc('\n', stderr);
    r = (Rect *)xv_get(xv_get(panel, CANVAS_NTH_PAINT_WINDOW, 0), WIN_MOUSE_XY);
    fprintf(stderr, "panel window: ");
    rect_print(r);
    fputc('\n', stderr);
}

int main(int argc, char * argv[])
{
    Frame        frame;
    Canvas       canvas;
    Xv_Cursor    cursor;

    xv_init(XV_INIT_ARGC_PTR_ARGV, &argc, argv);

    /*
     * create a cursor based on the image just created
     */
    cursor = xv_create(XV_NULL, CURSOR,
        CURSOR_SRC_CHAR,        OLC_STOP_PTR,
        0);

    /*
     * Create a base frame and a canvas
     */
    frame = xv_create(XV_NULL, FRAME, 0);
    canvas = xv_create(frame, PANEL,
        XV_WIDTH,               100,
        XV_HEIGHT,              100,
        0);
    /*
     * set the cursor to the paint window for the canvas
     * Do not set it for the canvas itself.
     */
    xv_set(xv_get(canvas, CANVAS_NTH_PAINT_WINDOW, 0),
        WIN_CURSOR,             cursor,
        0);
    xv_create(canvas, PANEL_BUTTON,
	PANEL_LABEL_STRING,	"Do It",
	PANEL_NOTIFY_PROC,	do_it,
	0);

    window_fit(frame);
    window_main_loop(frame);
}
