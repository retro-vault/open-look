/* $Id: alerts.c,v 1.4 91/12/08 15:33:39 pturner Exp Locker: pturner $
 *
 * yesno(), errwin()
 *
 */

#include <stdio.h>
#include <X11/Xlib.h>
#include <xview/xview.h>
#include <xview/panel.h>
#include <xview/notice.h>

extern Frame main_frame;

extern int inwin;		/* true if in window system */

/* alerts */

/*
 * popup for a yes/no response
 */
int yesno(char * msg1, char *msg2, char * msgyes, char *msgno)
{
    char buf[300];
    int result;
    Event event;		/* unused */
    int beep = 0;

    if (!inwin) {
	fprintf(stderr, "%s\n", msg1);
	fprintf(stderr, "%s\n", "abort? (y/n)");
	gets(buf);
	if (buf[0] == 'y') {
	    return 1;
	} else {
	    return 0;
	}
    }
    result = notice_prompt(main_frame, &event,
			   NOTICE_MESSAGE_STRINGS,
			   msg1, msg2,
			   0,
			   NOTICE_NO_BEEPING, (beep) ? 0 : 1,
			   NOTICE_BUTTON_YES, msgyes,
			   NOTICE_BUTTON_NO, msgno,
			   NOTICE_TRIGGER, ACTION_STOP,	/* allow either YES or
							 * NO answer */
			   0);
    switch (result) {
    case NOTICE_YES:
	return 1;
	break;
    case NOTICE_NO:
	return 0;
	break;
    case NOTICE_TRIGGERED:	/* result of ACTION_STOP trigger */
	break;
    case NOTICE_FAILED:	/* not likely to happen unless out of memory */
	break;
    }
/* hope it never gets here */
    return 0;
}

/*
 * display an error message
 */
void errwin(char * msg)
{
    int result;
    Event event;		/* unused */
    char *contine_msg = "Press \"Continue\" to proceed.";
    int beep = 0;

    if (!inwin) {
	fprintf(stderr, "%s\n", msg);
	return;
    }
    result = notice_prompt(main_frame, &event,
			   NOTICE_MESSAGE_STRINGS,
			   msg,
			   contine_msg,
			   0,
			   NOTICE_NO_BEEPING, (beep) ? 0 : 1,
			   NOTICE_BUTTON_YES, "Continue",
			   NOTICE_TRIGGER, ACTION_STOP,
			   0);
    switch (result) {
    case NOTICE_YES:
    case NOTICE_TRIGGERED:	/* result of ACTION_STOP trigger */
	break;
    case NOTICE_FAILED:	/* not likely to happen unless out of memory */
	break;
    }
}
