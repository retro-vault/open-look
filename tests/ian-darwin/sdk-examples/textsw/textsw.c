#include <xview/xview.h>
#include <xview/textsw.h>
#include <xview/panel.h>

#define HEIGHT		500
#define WIDTH		500

Textsw		textsw;
Panel           panel;
Icon		icon;

int main(int argc, char	* argv[])
{
    Frame	frame;
    int		textsw_width;

    xv_init(XV_INIT_ARGS, argc, argv, 0);

    frame = xv_create(XV_NULL, FRAME, FRAME_LABEL, argv[0], 0);

    /* create textsw and set the colormap segment for it */
    textsw = xv_create(frame, TEXTSW, 
	   WIN_ROWS,	20,
	   WIN_COLUMNS,	80,
	   0); 

    window_fit(frame);
    xv_main_loop(frame);
}
