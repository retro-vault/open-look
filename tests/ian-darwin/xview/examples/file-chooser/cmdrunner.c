#include <xview/ttysw.h>

/*
 * Demonstrate use of the History package
 */

#include <stdio.h>
#include <xview/xview.h>
#include <xview/panel.h>
#include <xview/tty.h>
#include <xview/hist.h>


static Attr_attribute MY_KEY;

typedef struct {
    Frame		frame;
    Panel		panel;
    Panel_text_item	cmd_txt;
    Panel_button_item	btn;
    Tty			tty;
    History_list	hl;
    History_menu	hm;
} My_ui;


static Panel_setting
my_text_notify(Panel_text_item item, Event * event)
{
    My_ui *ui = (My_ui *) xv_get(item, XV_KEY_DATA, MY_KEY);
    char *cmd = (char *) xv_get(item, PANEL_VALUE);

    (void) ttysw_input(ui->tty, cmd, strlen(cmd));
    (void) ttysw_input(ui->tty, "\n", 1);

    if ( cmd && *cmd )
	xv_set(ui->hl, HISTORY_ADD_ROLLING_ENTRY, cmd, cmd, 0);
    return PANEL_NONE;
}



static void
my_hist_notify(History_menu hm, char * label, char * value)
{
    My_ui *ui = (My_ui *) xv_get(hm, XV_KEY_DATA, MY_KEY);

    (void) ttysw_input(ui->tty, value, strlen(value));
    (void) ttysw_input(ui->tty, "\n", 1);
}



int
main(int argc, char ** argv)
{
    My_ui ui;

    (void) xv_init(XV_INIT_ARGC_PTR_ARGV, &argc, argv, 0);
    MY_KEY = xv_unique_key();


    ui.hl = xv_create(XV_NULL, HISTORY_LIST,
		      HISTORY_ADD_FIXED_ENTRY,		"Long Listing", "ls -l",
		      HISTORY_ADD_FIXED_ENTRY,		"Short Listing", "ls",
		      HISTORY_ADD_FIXED_ENTRY,		"Current Directory", "pwd",
		      HISTORY_ADD_FIXED_ENTRY,		"Home", "cd",
		      HISTORY_DUPLICATE_LABELS,		FALSE,
		      HISTORY_DUPLICATE_VALUES,		FALSE,
		      0);

    ui.hm = xv_create(XV_NULL, HISTORY_MENU,
		      HISTORY_MENU_HISTORY_LIST,	ui.hl,
		      HISTORY_NOTIFY_PROC,		my_hist_notify,
		      XV_KEY_DATA,			MY_KEY, &ui,
		      0);

    ui.frame = xv_create(XV_NULL, FRAME, 
			 XV_LABEL,	argv[0],
			 0);

    ui.panel = xv_create(ui.frame, PANEL, 0 );

    ui.btn = xv_create(ui.panel, PANEL_BUTTON,
		       PANEL_LABEL_STRING,	"Command:",
		       PANEL_ITEM_MENU,		xv_get(ui.hm, HISTORY_MENU_OBJECT),
		       0);

    ui.cmd_txt = xv_create(ui.panel, PANEL_TEXT,
			   PANEL_VALUE_DISPLAY_LENGTH,	64,
			   PANEL_NOTIFY_PROC,		my_text_notify,
			   XV_KEY_DATA,			MY_KEY, &ui,
			   0 );

    window_fit_height( ui.panel );

    ui.tty = xv_create(ui.frame, TTY,
		       TTY_QUIT_ON_CHILD_DEATH,	TRUE,
		       WIN_MENU,		xv_get(ui.hm, HISTORY_MENU_OBJECT),
		       0);
    
    xv_main_loop( ui.frame );
}
