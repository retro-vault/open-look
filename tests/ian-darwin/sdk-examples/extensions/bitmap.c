/* bitmap.c -- demonstrate the use of the Bitmap package. */
#include <xview/xview.h>
#include <xview/cms.h>
#include "bitmap.h"

int main(int argc, char * argv[])
{
    Frame frame;
    Cms cms;
    Bitmap bitmap;

    xv_init(XV_INIT_ARGC_PTR_ARGV, &argc, argv, 0);

    if (argc <= 1)
	puts("Specify bitmap filename"), exit(1);

    frame = (Frame)xv_create(0, FRAME, 0);
    cms = xv_create(0, CMS,
        CMS_SIZE,       2,
        CMS_NAMED_COLORS, "LightBlue", "maroon", 0,
        0);
    bitmap = xv_create(frame, BITMAP,
        XV_WIDTH,       100,
        XV_HEIGHT,      100,
        WIN_CMS,        cms,
	BITMAP_FILE,    argv[1],
        0);

    window_fit(frame);
    xv_main_loop(frame);
}
