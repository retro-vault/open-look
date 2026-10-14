/*
 * popup.c -- popup a frame and allow the user to interact with
 * the new popup frame.
 */
#include <xview/xview.h>
#include <xview/panel.h>

Frame frame;     /* top level application base-frame */
Frame subframe;  /* subframe (FRAME_CMD) is a child of frame */

int main(int argc, char * argv[])
{
    Panel panel;
    int show_cmd_frame(), pushed();

    xv_init(XV_INIT_ARGC_PTR_ARGV, &argc, argv, 0);

    /* Create base frame */
    frame = (Frame)xv_create(0, FRAME,
        FRAME_LABEL, argv[0],
        0);

    /* Install a panel and a panel button */
    panel = (Panel)xv_create(frame, PANEL, 0);
    (void) xv_create(panel, PANEL_BUTTON,
        PANEL_LABEL_STRING, "Hello",
        PANEL_NOTIFY_PROC,  show_cmd_frame,
        0);

    /* Create the command frame -- not displayed until XV_SHOW is set */
    subframe = (Frame)xv_create(frame, FRAME_CMD,
        FRAME_LABEL,         "Popup",
        0);

    /* Command frames have panels already created by default -- get it */
    panel = (Panel)xv_get(subframe, FRAME_CMD_PANEL);
    (void) xv_create(panel, PANEL_BUTTON,
        PANEL_LABEL_STRING,  "Push Me",
        PANEL_NOTIFY_PROC,   pushed,
        0);

    xv_main_loop(frame);
}

/* Called when base frame's button is pushed -- show/raise subframe */
int show_cmd_frame(Frame item, Event * event)
{
    xv_set(subframe, XV_SHOW, TRUE, 0);
}

/* Called when command frame's button is pushed */
int pushed(Panel_item item, Event  * event)
{
    printf("Hello world.\n");

    /* Check to see if the pushpin is in -- if not, close frame */
/*    if ((int)xv_get(subframe, FRAME_CMD_PUSHPIN_IN) == FALSE) */
    if ((int)xv_get(subframe, FRAME_CMD_PIN_STATE) == FRAME_CMD_PIN_OUT)
        xv_set(subframe, XV_SHOW, FALSE, 0);
}
