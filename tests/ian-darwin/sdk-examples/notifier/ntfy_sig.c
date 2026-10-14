/*
 * ntfy_sig.c -- shows how to catch signals using the notifier
 */
#include <xview/xview.h>
#include <xview/frame.h>
#include <xview/panel.h>

Frame frame;

Notify_value
sigint_handler(Notify_client client, int sig, int when)
{
    printf("Received signal %d\n", sig);
    return NOTIFY_DONE;
}

int main(int argc, char * argv[])
{
    Panel panel;
    char  buf[BUFSIZ];
    int   n, quit();

    xv_init (XV_INIT_ARGC_PTR_ARGV, &argc, argv, 0);

    frame = (Frame)xv_create (0, FRAME,
        FRAME_LABEL,    argv[0],
        XV_WIDTH,       200,
        XV_HEIGHT,      100,
        0);

    panel = (Panel)xv_create (frame, PANEL, 0);

    (void) xv_create (panel, PANEL_BUTTON,
            PANEL_LABEL_STRING,         "Quit",
            PANEL_NOTIFY_PROC,          quit,
            0);

    notify_set_signal_func(frame, sigint_handler, SIGINT, NOTIFY_ASYNC);

    xv_main_loop(frame);
}

int
quit(void)
{
    xv_destroy_safe(frame);
    return XV_OK;
}
