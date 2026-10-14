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

/*
#define MAX(a,b)	(((a)>(b))?(a):(b))
#define MIN(a,b)	(((a)<(b))?(a):(b))
*/


/* ---------------------------- Imports -------------------------------- */




/* ---------------------------- Locals --------------------------------- */


/*
	This is used only with exits
*/
int
save_or_not_save(Frame win, char    * s)
{
	return ( notice_prompt(
			(Frame)win,
			(Event *)		0,
			NOTICE_NO_BEEPING,	TRUE,
			NOTICE_MESSAGE_STRINGS,	s,0,
			NOTICE_BUTTON_YES,	"discard changes",
			NOTICE_BUTTON_NO,	"save changes",
			0) );
}

/*
	Show a popup window give a message and
	get an answer from the user.
*/
static
int do_pop(Frame win, char	* s, int show_cancel)
{
	int	result;
		if(show_cancel)
			result = notice_prompt(
					(Frame)win,
					(Event *)		0,
					NOTICE_NO_BEEPING,	TRUE,
					NOTICE_MESSAGE_STRINGS,	s,0,
					NOTICE_BUTTON_YES,	"ok",
					NOTICE_BUTTON_NO,	"cancel",
					0);
		else
			result = notice_prompt(
					(Frame)win,
					(Event *)		0,
					NOTICE_NO_BEEPING,	FALSE,
					NOTICE_MESSAGE_STRINGS,	s,0,
					NOTICE_BUTTON_YES,	"ok",
					0);
	return(result);
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

