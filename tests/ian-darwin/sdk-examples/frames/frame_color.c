/* 
 *    frame_color.c - demonstrates how to set the frame's
 *    foreground and make it propogate to the children of
 *    the frame.
 */

#include <xview/xview.h>
#include <xview/panel.h>
#include <xview/cms.h>

int main(int argc, char 	** argv)
{
    Frame         frame;
    Panel         panel;
    Cms		  cms;

    (void)xv_init(XV_INIT_ARGC_PTR_ARGV, &argc, argv, 0);

    frame = (Frame)xv_create(0, FRAME, 
	FRAME_LABEL,		argv[0],
	0);

    cms = xv_create(0, CMS,
	CMS_SIZE,		CMS_CONTROL_COLORS + 1,
	CMS_CONTROL_CMS,	True,
	CMS_NAMED_COLORS,	"red", 0,
	0);

    xv_set(frame, WIN_CMS, cms, 0);

    panel = (Panel)xv_create(frame, PANEL, 0);
    (void) xv_create(panel, PANEL_BUTTON,
        PANEL_LABEL_STRING,    "Push Me",
        0);

    xv_main_loop(frame);
}


