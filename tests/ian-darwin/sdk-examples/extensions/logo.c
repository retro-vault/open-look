/* logo.c -- demonstrate the use of the logo package. */
#include <xview/xview.h>
#include <xview/cms.h>
#include "logo.h"

int main(int argc, char * argv[])
{
    Frame frame;
    Cms cms;
    Logo logo;

    xv_init(XV_INIT_ARGC_PTR_ARGV, &argc, argv, 0);

    frame = (Frame)xv_create(0, FRAME, 0);
    cms = xv_create(0, CMS,
        CMS_SIZE,       2,
        CMS_NAMED_COLORS, "powder blue", "maroon", 0,
        0);
    logo = xv_create(frame, LOGO,
        XV_WIDTH,       100,
        XV_HEIGHT,      100,
        WIN_CMS,        cms,
        0);

    window_fit(frame);
    xv_main_loop(frame);
}
