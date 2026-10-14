#include <xview/xview.h>
#include <xview/frame.h>
#include <xview/cursor.h>
#include <xview/panel.h>

#include "robot.h"

static int	itext_fast, itext;
static int	ifast, islow;
static float	xpos, ypos;	
Frame		text_editor_frame;
Panel_item	text_editor_text;
Panel_item	text_editor_xpos;
Panel_item	text_editor_ypos;
Panel_item	text_editor_message;
Panel_item	text_editor_message2;

char		label[ILENGTH];
void
text_editor_ok(void)
{

	sscanf((char *)xv_get(text_editor_xpos, PANEL_VALUE), "%g", &xpos);
	sscanf((char *)xv_get(text_editor_ypos, PANEL_VALUE), "%g", &ypos);

	replace_text(islow, ifast, itext, itext_fast, xpos, ypos, 
		xv_get(text_editor_text, PANEL_VALUE));
	/* xv_set(text_editor_frame,
		FRAME_CMD_PUSHPIN_IN, FALSE, 
		0);
	xv_set(text_editor_frame,
		XV_SHOW, FALSE, 
		0); */
	redraw();
}



void
make_text_editor_frame(Frame frame)
{
	text_editor_frame = xv_create(frame, FRAME_CMD,
			FRAME_LABEL, "Modify Text",
			FRAME_CMD_PUSHPIN_IN, TRUE,
			0);
	text_editor_panel = xv_get(text_editor_frame, FRAME_CMD_PANEL);
	xv_set(text_editor_panel, PANEL_LAYOUT, PANEL_VERTICAL, 0);
	text_editor_message = xv_create(text_editor_panel, PANEL_MESSAGE,
					0);
	text_editor_message2 = xv_create(text_editor_panel, PANEL_MESSAGE,
					0);
	text_editor_text = xv_create(text_editor_panel, PANEL_TEXT,
			PANEL_LABEL_STRING, "Text is:",
			PANEL_VALUE_DISPLAY_LENGTH, 25,
			XV_HELP_DATA,	"robot:text_editor_text",
			0);
	text_editor_xpos = xv_create(text_editor_panel, PANEL_TEXT,
			PANEL_LABEL_STRING, "x position:",
			PANEL_VALUE_DISPLAY_LENGTH, 25,
			XV_HELP_DATA,	"robot:text_editor_xpos",
			0);
	text_editor_ypos = xv_create(text_editor_panel, PANEL_TEXT,
			PANEL_LABEL_STRING, "y position:",
			PANEL_VALUE_DISPLAY_LENGTH, 25,
			XV_HELP_DATA,	"robot:text_editor_ypos",
			PANEL_NOTIFY_PROC,	text_editor_ok,
			0);
	xv_set(text_editor_panel, PANEL_DEFAULT_ITEM,
		xv_create(text_editor_panel, PANEL_BUTTON,
			PANEL_LABEL_STRING,	"Apply Change",
			PANEL_NOTIFY_PROC, text_editor_ok,
			XV_HELP_DATA,	"robot:text_editor_button",
			0),
		0);
if(not_open_look){
	
           xv_set(text_editor_panel,
		PANEL_LAYOUT, PANEL_HORIZONTAL,
		0);

	   xv_create(text_editor_panel, PANEL_BUTTON,
		XV_HELP_DATA,		"robot:dismiss",
		PANEL_LABEL_STRING,	DISMISS,
		PANEL_NOTIFY_PROC,	hide_me,
		0);
}
	window_fit(text_editor_panel);
	window_fit(text_editor_frame);
			
}



void
show_text_editor(int is, int ifst, int it, int itf, float xfound, float yfound, char	* text)
{
	islow = is;
	ifast = ifst;
	itext_fast = itf;
	itext = it;
	sprintf(label, "x = %g y = %g", xfound, yfound);
	xv_set(text_editor_text, PANEL_VALUE, text, 0);
	xv_set(text_editor_message, PANEL_LABEL_STRING, text, 0);
	xv_set(text_editor_message2, PANEL_LABEL_STRING, label, 0);
	sprintf(label, "%g", xfound);
	xv_set(text_editor_xpos, PANEL_VALUE, label, 0);
	sprintf(label, "%g", yfound);
	xv_set(text_editor_ypos, PANEL_VALUE, label, 0);
	xv_set(text_editor_frame, 
		XV_SHOW, TRUE, 
		FRAME_CMD_PUSHPIN_IN, TRUE,
		0);
}

Bool
text_editor_update(void)
{
	if(xv_get(text_editor_frame, XV_SHOW) == TRUE)
		return True;
	else
		return False;
}
