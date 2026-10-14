#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <unistd.h>
#ifndef lint
static char sccsid[] = "@(#)popup.c	2.2 8/12/88";
#endif

/*
 *	popup - pop up error dialog windows
 */


#include <xview/xview.h>
#include <xview/panel.h>
#include <xview/notice.h>
#include <stdio.h>


#define X_GAP		25
#define Y_GAP		30

#ifndef MAX
#define MAX(a,b)	(((a)>(b))?(a):(b))
#endif
#ifndef MIN
#define MIN(a,b)	(((a)<(b))?(a):(b))
#endif


/* ---------------------------- Imports -------------------------------- */

#if !defined(sgi) && !defined(hpux)

#endif

static
int do_pop(Frame frame, char	* s, int show_cancel)
{
	int	answer;

	if ( show_cancel )
	  answer = notice_prompt(frame, 0,
							 NOTICE_MESSAGE_STRINGS, s, 0,
							 NOTICE_BUTTON_YES, "   OK   ",
							 NOTICE_BUTTON_NO,  " Cancel ",
							 0);
	else
	  answer = notice_prompt(frame, 0,
							 NOTICE_MESSAGE_STRINGS, s, 0,
							 NOTICE_BUTTON_YES, "   OK   ",
							 0);
	  
	return (answer == NOTICE_YES ? 1 : 0);
}


/*
 *	Glue routine for a confirmer dialog box.  The first argument is
 *	the frame relative to which the dialog window is displayed.  The
 *	remaining arguments are a format string and generic args for
 *	the sprintf call.  The flag is passed to the dialog frame creation
 *	routine which indicates that both the OK and CANCEL buttons are
 *	to be displayed.
 */

/*VARARGS2*/
int pop_confirm(Frame frame, char *format, ...)
{
    char message[512];
    va_list args;
    va_start(args, format);
    vsnprintf(message, sizeof(message), format, args);
    va_end(args);
    return do_pop(frame, message, TRUE);
}


/*
 *	Glue routine for a simple info popup frame.  Only the OK button is
 *	displayed and the frame waits until it is clicked.
 */

/*VARARGS2*/
int pop_msg(Frame frame, char *format, ...)
{
    char message[512];
    va_list args;
    va_start(args, format);
    vsnprintf(message, sizeof(message), format, args);
    va_end(args);
    return do_pop(frame, message, FALSE);
}

