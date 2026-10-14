#include <xview/xview.h>

int main(int argc, char * argv[])
{
    Frame frame, subframe;

    frame = xv_create(0, FRAME,
        FRAME_ARGC_PTR_ARGV,    &argc, argv,
        XV_WIDTH,                       100,
        XV_HEIGHT,              100,
        FRAME_LABEL,            "Base Frame",
        0);

    subframe = xv_create(frame, FRAME_CMD,
        XV_WIDTH,                       100,
        XV_HEIGHT,              100,
        FRAME_LABEL,            "Popup",
        0);

    xv_set(subframe, XV_SHOW, TRUE, 0);

    xv_main_loop(frame);
}
