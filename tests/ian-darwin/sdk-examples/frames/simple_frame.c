#include <xview/xview.h>

int main(void)
{
    Frame frame;
    frame = (Frame)xv_create(0, FRAME, 0);
    xv_main_loop(frame);
}
