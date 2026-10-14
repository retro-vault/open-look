#include <errno.h>
#include <sys/stat.h>
#include <unistd.h>
/*
 * xv_ok_script.c -- display a script and ask if it's OK to execute
 */

#include <xview/xview.h>
#include <xview/panel.h>
#include <xview/textsw.h>
#include <xview/notice.h>
#include <sys/errno.h>

char	*filename;
Panel	panel;
Textsw	textsw;
Panel_item message;

int doit(void)
{
	char *msg = "Could not run script under /bin/sh", *reason;
	
	int result;

	/* Mosaic tends to copy files & lose modes, so try chmod */
	(void)chmod(filename, 0755);

	execl(filename, filename, 0);

	/* if not #!, try running it under sh */
	if (errno == ENOEXEC) {
		execl("/bin/sh", "default-sh", filename, 0);
		msg =  "Could not run script under /bin/sh";
	}

	/* Exec failed, say why, and abort. */
	reason = strerror(errno);
	
	result = notice_prompt(panel, 0,
		NOTICE_MESSAGE_STRINGS,
			msg, filename, reason, 0,
		NOTICE_BUTTON_YES,      "OK",
		0);

	exit(1);
}

int quit(void)
{
	exit(0);
}

int main(int argc, char    * argv[])
{
	Frame       frame;
	char *msg = "Do you wish to run this script?\n";
	Textsw_status status;

	/*
	 * Initialize XView, having it eat all args so we just get filename
	 */
	xv_init(XV_INIT_ARGC_PTR_ARGV, &argc, argv, 0);

	if (argc != 2) {
		fprintf(stderr, "Usage: %s script-file-to-OK\n", argv[0]);
		exit(1);
	}

	filename = argv[1];
	frame = (Frame)xv_create(XV_NULL, FRAME,
		XV_LABEL, filename,
		0);
	panel = (Panel)xv_create(frame, PANEL, 0);
	message = (Panel_item)xv_create(panel, PANEL_MESSAGE,
		PANEL_LABEL_STRING, msg,
		0);
	textsw = (Textsw)xv_create(panel, TEXTSW, 
		XV_X, 0, XV_Y, 30,
		XV_HEIGHT, 200,
		XV_WIDTH, 400,
		0);
	xv_set(textsw, TEXTSW_FILE, filename, 0);
	xv_set(textsw, TEXTSW_READ_ONLY, 0);

	xv_create(panel, PANEL_BUTTON,
		XV_X,  20, XV_Y, 240,
		PANEL_LABEL_STRING, "Yes, Run Script",
		PANEL_NOTIFY_PROC,  doit,
		0);

	xv_create(panel, PANEL_BUTTON,
		XV_X, 250, XV_Y, 240,
		PANEL_LABEL_STRING, "NO, do not run script",
		PANEL_NOTIFY_PROC,  quit,
		0);

	window_fit(panel);
	window_fit(frame);

	xv_main_loop(frame);

	exit(0);
}
