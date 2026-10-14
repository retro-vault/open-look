/* logo.c -- demonstrate the use of the image package. */
#include <xview/xview.h>
#include "image.h"

int main(int argc, char * argv[])
{
    Frame frame;
    Image image1, image2;
    Pixmap pixmap;

    xv_init(XV_INIT_ARGC_PTR_ARGV, &argc, argv, 0);

    if (argc < 2)
	puts("specify filename"), exit(1);

    /* frame = (Frame)xv_create(0, FRAME, 0); */
    if (!(image1 = xv_create(0, IMAGE,
        XV_WIDTH,                  100,
        XV_HEIGHT,                 100,
        SERVER_IMAGE_BITMAP_FILE,  argv[1],
        0)))
	    puts("unsuccessfully created image1"), exit(1);
    if (!(image2 = xv_find(0, IMAGE,
        SERVER_IMAGE_BITMAP_FILE,  argv[1],
	0)))
	    puts("unsuccessfully created image2"), exit(1);
    printf("image1 %s image2\n",
	(image2 != image1)? "matched" : "didn't match");
    pixmap = (Pixmap)xv_get(image1, SERVER_IMAGE_PIXMAP);
    if (!(image2 = xv_find(0, IMAGE,
        SERVER_IMAGE_PIXMAP,  pixmap,
	0)))
	    puts("unsuccessfully created image2"), exit(1);
    printf("image1 %s image2\n",
	(image2 != image1)? "matched" : "didn't match");

    /* window_fit(frame); */
    /* xv_main_loop(frame); */
}
