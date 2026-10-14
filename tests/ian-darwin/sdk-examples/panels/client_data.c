/*
 * client_data.c -- demonstrate the use of PANEL_CLIENT_DATA attached
 * to panel items.  Attach the base frame to the "Quit" panel item so
 * that the notify procedure can call xv_destroy_safe() on the frame.
 */
#include <xview/xview.h>
#include <xview/panel.h>

int main(int argc, char * argv[])
{
    Frame  frame;
    Panel  panel;
    int    quit();

    xv_init(XV_INIT_ARGC_PTR_ARGV, &argc, argv, 0);

    frame = (Frame)xv_create(XV_NULL, FRAME, 0);
    panel = (Panel)xv_create(frame, PANEL, 0);
    (void) xv_create(panel, PANEL_BUTTON,
        PANEL_LABEL_STRING,     "Quit",
        PANEL_NOTIFY_PROC,      quit,
        PANEL_CLIENT_DATA,      frame,
        0);

    xv_main_loop(frame);
    puts("The program is now done.");
    exit(0);
}

int quit(Panel_item item)
{
    Frame frame = (Frame)xv_get(item, PANEL_CLIENT_DATA);
    xv_destroy_safe(frame);
}
