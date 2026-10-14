/*
 * stop.c -- Use a server image as a panel message item.
 */
#include <xview/xview.h>
#include <xview/svrimage.h>
#include <xview/panel.h>

static unsigned short stop_bits[] = {
    0x3E00, 0x7F00, 0xFF80, 0xFF80, 0xFF80, 0xFF80, 0xFF80, 0x7F00,
    0x3E00, 0x0800, 0x0800, 0x0800, 0x0800, 0x0800, 0x7F00, 0x0000
};

int main(int argc, char * argv[])
{
    Frame frame;
    Panel panel;
    Server_image stopsign;

    xv_init(XV_INIT_ARGC_PTR_ARGV, &argc, argv, 0);

    frame = (Frame)xv_create(0, FRAME,
        FRAME_SHOW_HEADER,  FALSE,
        0);
    panel = (Panel)xv_create(frame, PANEL, 0);

    stopsign = (Server_image)xv_create(0, SERVER_IMAGE,
        XV_WIDTH,           16,
        XV_HEIGHT,          16,
        SERVER_IMAGE_DEPTH, 1,
        SERVER_IMAGE_BITS,  stop_bits,
        0);

    (void) xv_create(panel, PANEL_MESSAGE,
        PANEL_LABEL_IMAGE,  stopsign,
        0);
    (void) xv_create(panel, PANEL_MESSAGE,
        PANEL_LABEL_STRING,
            "This action will cause unsaved edits to be lost.",
        0);
    (void)xv_create(panel, PANEL_BUTTON,
        PANEL_NEXT_ROW,     -1,
        XV_X,               110,
        PANEL_LABEL_STRING, "Ok",
        0);
    (void)xv_create(panel, PANEL_BUTTON,
        PANEL_LABEL_STRING, "Cancel",
        0);

    window_fit(panel);
    window_fit(frame);
    xv_main_loop(frame);
}
