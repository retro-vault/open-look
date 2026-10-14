
#include <stdio.h>
#include <unistd.h>
#include <sys/param.h>
#include <xview/xview.h>
#include <xview/panel.h>
#include <xview/path.h>


static Attr_attribute MY_KEY;

typedef struct {
    Frame frame;
    Panel panel;
    Path_name path;
    Panel_choice_item path_choice;
} My_ui;

static Panel_setting 	my_path_notify();
static void		my_choice_notify();

int
main(int argc, char ** argv)
{
    My_ui ui;
    char *cwd = (char *)getcwd(0, MAXPATHLEN);

    (void) xv_init(XV_INIT_ARGC_PTR_ARGV, &argc, argv, 0);
    MY_KEY = xv_unique_key();


    ui.frame = xv_create(XV_NULL, FRAME,
			 XV_LABEL,		argv[0],
			 FRAME_SHOW_FOOTER,	TRUE,
			 0 );

    ui.panel = xv_create(ui.frame, PANEL,
			 PANEL_LAYOUT,	PANEL_VERTICAL,
			 0 );

    ui.path = xv_create (ui.panel, PATH_NAME,
			 PANEL_VALUE_DISPLAY_LENGTH,	32,
			 PANEL_LABEL_STRING,		"Path:",
			 PATH_USE_FRAME,		TRUE,
			 PATH_RELATIVE_TO,		cwd,
			 PANEL_NOTIFY_PROC,		my_path_notify,
			 XV_KEY_DATA,			MY_KEY, &ui,
			 0 );
    free( cwd );

    ui.path_choice = xv_create(ui.panel, PANEL_CHOICE,
			       PANEL_LABEL_STRING,	"Path Is:",
			       PANEL_CHOICE_STRINGS,	"Any File", "Directory", 0,
			       PANEL_NOTIFY_PROC,	my_choice_notify,
			       XV_KEY_DATA,		MY_KEY, &ui,
			       0);

    window_fit( ui.panel );
    window_fit( ui.frame );
    xv_main_loop( ui.frame );
}



static Panel_setting
my_path_notify(Path_name path, Event * event, struct stat * stats)
{
    My_ui *ui = (My_ui *)xv_get(path, XV_KEY_DATA, MY_KEY);
    char *new_path = (char *)xv_get(path, PATH_LAST_VALIDATED);

    if ( !stats )
	fprintf(stderr, "New File\n");

    if ( new_path )
	xv_set(ui->frame,
	       FRAME_LEFT_FOOTER,  new_path,
	       0);

    return PANEL_NEXT;
}


static void
my_choice_notify(Panel_item item, int value, Event * event)
{
    My_ui *ui = (My_ui *)xv_get(item, XV_KEY_DATA, MY_KEY);

    xv_set(ui->path, PATH_IS_DIRECTORY,	value, 0);
}
