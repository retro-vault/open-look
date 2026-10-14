
#include <stdio.h>
#include <xview/xview.h>
#include <xview/panel.h>
#include <xview/file_list.h>


static Attr_attribute	MY_KEY;
static int		my_file_list_notify();
static int		my_compare_func();

int
main(int argc, char ** argv)
{
    Frame frame;
    Panel panel;

    (void) xv_init(XV_INIT_ARGC_PTR_ARGV, &argc, argv, 0);
    MY_KEY = xv_unique_key();

    frame = xv_create(XV_NULL, FRAME,
		      XV_LABEL,			argv[0],
		      FRAME_SHOW_FOOTER,	TRUE,
		      0);

    panel = xv_create(frame, PANEL,
		      PANEL_LAYOUT,	PANEL_VERTICAL,
		      0 );

    (void) xv_create(panel, FILE_LIST,
		     PANEL_LIST_WIDTH,		300,
		     PANEL_NOTIFY_PROC,		my_file_list_notify,
		     FILE_LIST_USE_FRAME,	TRUE,
		     FILE_LIST_SHOW_DIR,	TRUE,
		     FILE_LIST_COMPARE_FUNC,	my_compare_func,
		     XV_KEY_DATA,		MY_KEY, frame,
		     0);

    window_fit( panel );
    window_fit( frame );
    xv_main_loop( frame );
}


static int
my_file_list_notify(File_list item, char * dir, char * file, Xv_opaque client_data, Panel_list_op op, Event * event, int row)
{
    Frame frame = (Frame) xv_get(item, XV_KEY_DATA, MY_KEY);

    if ( op == PANEL_LIST_OP_DBL_CLICK )
	xv_set(frame, FRAME_LEFT_FOOTER, file, 0);
    return XV_OK;
}



static int
my_compare_func(File_list_row * row1, File_list_row * row2)
{
    /* sort by last-modification time */
    return row1->stats.st_mtime < row2->stats.st_mtime;
}
