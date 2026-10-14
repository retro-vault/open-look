/*
 * xvnotice.c -- remind somebody that it's time for something.
 */
#include <unistd.h>

#include <xview/xview.h>
#include <xview/panel.h>
#include <xview/notice.h>

Panel       panel;

int main(int argc, char    * argv[])
{
    Frame       frame;
    int         result;

    /*
     * Initialize XView, create a frame, a panel and one panel button.
     */
    xv_init(XV_INIT_ARGS, argc, argv, 0);

    frame = (Frame)xv_create(XV_NULL, FRAME,
	XV_SHOW, FALSE,
	0);
    panel = (Panel)xv_create(frame, PANEL, 0);

    /* make sure everything looks good */
    window_fit(panel);
    window_fit(frame);

	do {
		result = notice_prompt(panel, 0,
			NOTICE_MESSAGE_STRINGS,
				argv[1]?argv[1]:"The time has come!", 0,
			NOTICE_BUTTON_YES,      "Yes",
			NOTICE_BUTTON_NO,       "No",
			0);
		if (result == NOTICE_NO)
			sleep(60);
	} while (result != NOTICE_YES);

    exit(0);
}
