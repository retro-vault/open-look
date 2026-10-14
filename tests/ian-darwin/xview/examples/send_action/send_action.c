/* Extracted from xview-examples.send-action; see adjacent archived message for author. */
/*
 *  The Tankster, 7/2/92
 *  Purpose:	Demonstrate that the caret may be positioned in a Panel
 * 		Text Item without any modifications to the library.
 *
 *  Note:	For best results, run with OpenWindows.KeyboardCommands
 *		set to something other than "Full" (i.e. Basic or SunView1).
 *		(e.g. caret_pos -xrm "OpenWindows.KeyboardCommands:Basic")
 */

#include <stdio.h>
#include <xview/xview.h>
#include <xview/panel.h>


static void send_action( Panel_item item, short action );
static void go_forward ( Panel_item item, Event *event );
static void go_backward ( Panel_item item, Event *event );

static Attr_attribute PTXT_KEY;
static Attr_attribute PCH_KEY;

#define BY_CHAR		0
#define BY_WORD		1
#define BY_LINE		2


int
main ( int argc, char **argv )
{
    Frame frame;
    Panel panel;
    Panel_text_item ptxt;
    Panel_choice_item pchoice;

    (void) xv_init ( XV_INIT_ARGC_PTR_ARGV, &argc, argv, NULL );
    PTXT_KEY = xv_unique_key();
    PCH_KEY = xv_unique_key();

    frame = xv_create ( XV_NULL, FRAME,
		       XV_LABEL,	"Move Caret",
		       NULL );

    panel = xv_create ( frame, PANEL, NULL );

    ptxt = xv_create ( panel, PANEL_TEXT,
		      PANEL_VALUE_DISPLAY_LENGTH,	16,
		      PANEL_LABEL_STRING,		"Text:",
		      PANEL_VALUE,			"This Is A Test",
		      NULL );

    send_action(ptxt, ACTION_LINE_START);


    pchoice = xv_create (panel, PANEL_CHOICE,
			 PANEL_NEXT_ROW,	-1,
			 PANEL_LABEL_STRING,	"By:",
			 PANEL_CHOICE_STRINGS,	"Char", "Word", "Line", NULL,
			 NULL);


    (void) xv_create ( panel, PANEL_BUTTON,
		      PANEL_NEXT_ROW,			-1,
		      PANEL_LABEL_STRING,		"<",
		      PANEL_NOTIFY_PROC,		go_backward,
		      XV_KEY_DATA,			PTXT_KEY, ptxt,
		      XV_KEY_DATA,			PCH_KEY, pchoice,
		      NULL );


    (void) xv_create ( panel, PANEL_BUTTON,
		      PANEL_LABEL_STRING,		">",
		      PANEL_NOTIFY_PROC,		go_forward,
		      XV_KEY_DATA,			PTXT_KEY, ptxt,
		      XV_KEY_DATA,			PCH_KEY, pchoice,
		      NULL );


    window_fit ( panel );
    window_fit ( frame );
    xv_main_loop ( frame );
}



/* 
 * Send synthetic semantic action to a panel item
 *
 * get the item's default event handler and synthesize a given
 * semantic action that cause the caret to be re-positioned.
 * some relevant actions are:  ACTION_LINE_START, ACTION_LINE_END,
 * ACTION_GO_WORD_FORWARD/BACKWARD, ACTION_GO_CHAR_FORWARD/BACKWARD.
 */
static void
send_action( Panel_item item, short action )
{
    Event event;
    void (* default_event)() 
	= (void (*)()) xv_get(item, PANEL_EVENT_PROC);

    event_init(&event);
    event_set_action(&event, action);
    (* default_event)(item, &event);
}



/*
 * Send fake ACTION_GO_CHAR_FORWARD to text item.
 */
void
go_forward ( Panel_item item, Event *event )
{
    Panel_text_item ptxt = xv_get(item, XV_KEY_DATA, PTXT_KEY);
    Panel_choice_item pc = xv_get(item, XV_KEY_DATA, PCH_KEY);
    int value = (int) xv_get(pc, PANEL_VALUE);

    switch ( value ) {
    case BY_CHAR:
	send_action(ptxt, ACTION_GO_CHAR_FORWARD);
	break;
    case BY_WORD:
	send_action(ptxt, ACTION_GO_WORD_FORWARD);
	break;
    case BY_LINE:
	send_action(ptxt, ACTION_LINE_END);
	break;
    }
}



/*
 * Send fake ACTION_GO_CHAR_BACKWARD to text item.
 */
void
go_backward ( Panel_item item, Event *event )
{
    Panel_text_item ptxt = xv_get(item, XV_KEY_DATA, PTXT_KEY);
    Panel_choice_item pc = xv_get(item, XV_KEY_DATA, PCH_KEY);
    int value = (int) xv_get(pc, PANEL_VALUE);

    switch ( value ) {
    case BY_CHAR:
	send_action(ptxt, ACTION_GO_CHAR_BACKWARD);
	break;
    case BY_WORD:
	send_action(ptxt, ACTION_GO_WORD_BACKWARD);
	break;
    case BY_LINE:
	send_action(ptxt, ACTION_LINE_START);
	break;
    }
}
