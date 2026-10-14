/*
 * interpose.c -- shows how to use an interpose destroy function
 */
#include <xview/xview.h>
#include <xview/frame.h>
#include <xview/panel.h>
#include <xview/notice.h>

Frame frame;

Notify_value
destroy_func(Notify_client client, Destroy_status status)
{
    if (status == DESTROY_CHECKING) {
        int answer = notice_prompt(client, 0,
            NOTICE_MESSAGE_STRINGS, "Really Quit?", 0,
            NOTICE_BUTTON_YES,  "No",
            NOTICE_BUTTON_NO,   "Yes",
            0);
        if (answer == NOTICE_YES)
            notify_veto_destroy(client);
    } else if (status == DESTROY_CLEANUP) {
        puts("cleaning up");
        /* allow frame to be destroyed */
        return notify_next_destroy_func(client, status);
    } else if (status == DESTROY_SAVE_YOURSELF)
        puts("save yourself?");
    else
        puts("process death");
    return NOTIFY_DONE;
}

int main(int argc, char * argv[])
{
    Panel panel;
    int   quit();

    xv_init (XV_INIT_ARGC_PTR_ARGV, &argc, argv, 0);

    frame = (Frame)xv_create (0, FRAME,
        FRAME_LABEL,    argv[0],
        XV_WIDTH,       200,
        XV_HEIGHT,      100,
        0);
    notify_interpose_destroy_func(frame, destroy_func);

    panel = (Panel)xv_create (frame, PANEL, 0);
    (void) xv_create (panel, PANEL_BUTTON,
            PANEL_LABEL_STRING,         "Quit",
            PANEL_NOTIFY_PROC,          quit,
            0);
    xv_main_loop(frame);
}

int
quit(void)
{
    xv_destroy_safe(frame);
    return XV_OK;
}
