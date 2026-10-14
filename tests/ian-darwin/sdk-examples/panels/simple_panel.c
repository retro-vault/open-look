/*
 * simple_panel.c -- program to display a panel button that says "Quit".
 * Selecting the panel button exits the program.
 */
#include <xview/xview.h>
#include <xview/frame.h>
#include <xview/panel.h>

int main(int argc, char * argv[])
{
    Frame  frame;
    Panel  panel;
    void   quit();

    xv_init (XV_INIT_ARGC_PTR_ARGV, &argc, argv, 0);

    frame = (Frame)xv_create(0, FRAME,
        FRAME_LABEL,    argv[0],
        XV_WIDTH,       200,
        XV_HEIGHT,      100,
        0);

    panel = (Panel)xv_create(frame, PANEL, 0);

    (void) xv_create(panel, PANEL_BUTTON,
        PANEL_LABEL_STRING,  "Quit",
        PANEL_NOTIFY_PROC,   quit,
        0);

    xv_main_loop(frame);
}

void
quit(void)
{
    exit(0);
}
