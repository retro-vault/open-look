
/*
 * Demonstrate an extended XView File Chooser in all 4 flavors
 */

#include <stdio.h>
#include <xview/xview.h>
#include <xview/panel.h>
#include <xview/textsw.h>
#include <xview/scrollbar.h>
#include <xview/file_chsr.h>


static Attr_attribute	MY_KEY;
static Attr_attribute	EXTEN_ITEM_KEY;
static Attr_attribute	PARENT_FC_KEY;

typedef struct {
    Frame		frame;
    Panel		panel;
    Panel_button_item	file_button;
    Menu		file_menu;
    Textsw		textsw;
    File_chooser	open;
    File_chooser	save;
    File_chooser	saveas;
    File_chooser	import;
    char *		doc_name;
    Panel_choice_item	open_dot_chs;
    Panel_choice_item	import_dot_chs;
    Panel_choice_item	save_dot_chs;
    Panel_choice_item	saveas_dot_chs;
} My_ui;

static void	my_open_notify();
static void	my_save_notify();
static void	my_saveas_notify();
static void	my_import_notify();
static int	my_open_callback();
static int	my_save_callback();
static int	my_exten_func();
static void	my_show_dot_files_proc();
static void	add_exten_item();


int
main(int argc, char ** argv)
{
    My_ui ui = {0};


    (void) xv_init ( XV_INIT_ARGC_PTR_ARGV, &argc, argv, 0 );
    MY_KEY = xv_unique_key();

    ui.file_menu 
	= xv_create(XV_NULL, MENU,
		    MENU_ITEM,
		    	MENU_STRING,		"Open...",
		    	MENU_NOTIFY_PROC,	my_open_notify,
		    	0,
		    MENU_ITEM,
		    	MENU_STRING,		"Import...",
		    	MENU_NOTIFY_PROC,	my_import_notify,
		    	0,
		    MENU_ITEM,
		    	MENU_STRING,		"Save...",
		    	MENU_NOTIFY_PROC,	my_save_notify,
		    	0,
		    MENU_ITEM,
		    	MENU_STRING,		"Save As...",
		    	MENU_NOTIFY_PROC,	my_saveas_notify,
		    	0,
		    XV_KEY_DATA,		MY_KEY, &ui,
		    0);

    ui.frame = xv_create(XV_NULL, FRAME,
			 XV_LABEL,		"Demo Text Editor",
			 FRAME_SHOW_FOOTER,	TRUE,
			 0 );

    ui.panel = xv_create(ui.frame, PANEL, 0);

    ui.file_button = xv_create(ui.panel, PANEL_BUTTON,
			       PANEL_LABEL_STRING,	"File",
			       PANEL_ITEM_MENU,		ui.file_menu,
			       0);

    window_fit_height( ui.panel );

    ui.textsw = xv_create(ui.frame, TEXTSW,
			  XV_X,		0,
			  WIN_BELOW,	ui.panel,
			  0);

    xv_main_loop( ui.frame );
    exit( 0 );
}




/*
 * Picked "Open" off of File Menu.
 */
static void
my_open_notify(Menu menu, Menu_item mi)
{
    My_ui *ui = (My_ui *)xv_get(menu, XV_KEY_DATA, MY_KEY);

    if ( !ui->open ) {
	ui->open 
	    = xv_create(ui->frame, FILE_CHOOSER_OPEN_DIALOG,
			XV_LABEL,			"Text Editor:  Open",
			FILE_CHOOSER_NOTIFY_FUNC,	my_open_callback,
			XV_KEY_DATA,			MY_KEY, ui,
			0);

	add_exten_item( ui->open, &ui->open_dot_chs );
    }

    xv_set(ui->open, XV_SHOW, TRUE, 0);
}





/*
 * Picked "Import" off of File Menu.
 */
static void
my_import_notify(Menu menu, Menu_item mi)
{
    My_ui *ui = (My_ui *)xv_get(menu, XV_KEY_DATA, MY_KEY);

    if ( !ui->import ) {
	ui->import 
	    = xv_create(ui->frame, FILE_CHOOSER_OPEN_DIALOG,
			XV_LABEL, 			"Text Editor:  Import",
			FILE_CHOOSER_NOTIFY_FUNC, 	my_open_callback,
			FILE_CHOOSER_CUSTOMIZE_OPEN, 
				"Import", 
				"Click Select to Import file",
				FILE_CHOOSER_SELECT_FILES,
			XV_KEY_DATA, 			MY_KEY, ui,
			0);

	add_exten_item( ui->import, &ui->import_dot_chs );
    }

    xv_set(ui->import, XV_SHOW, TRUE, 0);
}





/*
 * Picked "Save" off of File Menu.
 */
static void
my_save_notify(Menu menu, Menu_item mi)
{
    My_ui *ui = (My_ui *)xv_get(menu, XV_KEY_DATA, MY_KEY);

    if ( !ui->save ) {
	ui->save 
	    = xv_create(ui->frame, FILE_CHOOSER_SAVE_DIALOG,
			XV_LABEL, 			"Text Editor:  Save",
			FILE_CHOOSER_NOTIFY_FUNC, 	my_save_callback,
			XV_KEY_DATA, 			MY_KEY, ui,
			0);

	add_exten_item( ui->save, &ui->save_dot_chs );
    }

    xv_set(ui->save, XV_SHOW, TRUE, 0);
}



/*
 * Picked "Save As" off of File Menu.
 */
static void
my_saveas_notify(Menu menu, Menu_item mi)
{
    My_ui *ui = (My_ui *)xv_get(menu, XV_KEY_DATA, MY_KEY);

    if ( !ui->saveas ) {
	ui->saveas 
	    = xv_create(ui->frame, FILE_CHOOSER_SAVEAS_DIALOG,
			XV_LABEL, 			"Text Editor:  Save As",
			FILE_CHOOSER_NOTIFY_FUNC, 	my_save_callback,
			XV_KEY_DATA, 			MY_KEY, ui,
			0);

	add_exten_item( ui->saveas, &ui->saveas_dot_chs );

	if ( ui->doc_name ) {
	    xv_set(ui->saveas,
		   FILE_CHOOSER_DOC_NAME, ui->doc_name,
		   0);
	    free( ui->doc_name );
	    ui->doc_name = 0;
	}
    }

    xv_set(ui->saveas, XV_SHOW, TRUE, 0);
}




/*
 * Create "Show Hidden Files" item on a given File Chooser.
 */
static void
add_exten_item(File_chooser fc, Panel_item * item)
{
    Panel panel;
    int item_width;
    int item_height;
    int frame_width;
    int frame_height;

    
    panel = xv_get(fc, FRAME_CMD_PANEL);
    
    *item = xv_create(panel, PANEL_CHOICE,
		      PANEL_LABEL_STRING,	"Hidden Files:",
		      PANEL_CHOICE_STRINGS,	"Hide", "Show", 0,
		      PANEL_NOTIFY_PROC,	my_show_dot_files_proc,
		      XV_KEY_DATA,		PARENT_FC_KEY, fc,
		      0);
    
    item_width = (int) xv_get(*item, XV_WIDTH);
    item_height = (int) xv_get(*item, XV_HEIGHT);

    
    /*
     * Adjust Frame default size to make room for
     * the extension item.
     */
    frame_width = (int) xv_get(fc, XV_WIDTH);
    frame_height = (int) xv_get(fc, XV_HEIGHT);
    xv_set(fc, 
	   XV_WIDTH,  MAX(frame_width, (item_width + xv_cols(panel, 4))),
	   XV_HEIGHT, frame_height + item_height, 
	   0);


    /*
     * Adjust Frame Min Size.  provide for at least 2
     * columns on either side of the extension item.
     */
    xv_get(fc, FRAME_MIN_SIZE, &frame_width, &frame_height);
    xv_set(fc,
	   FRAME_MIN_SIZE, 
	   	MAX( frame_width, (item_width + xv_cols(panel, 4))),
	   	frame_height + item_height,
	   0);
    

    /* Tell File Chooser to reserve layout space for it */
    xv_set(fc,
	   FILE_CHOOSER_EXTEN_HEIGHT,	item_height,
	   FILE_CHOOSER_EXTEN_FUNC,	my_exten_func,
	   XV_KEY_DATA,			EXTEN_ITEM_KEY, *item,
	   0);
}



/*
 * Picked the Open button
 */
static int
my_open_callback(File_chooser fc, char * path, char * file, Xv_opaque client_data)
{
    My_ui *ui = (My_ui *)xv_get(fc, XV_KEY_DATA, MY_KEY);
    Textsw_status status;
    char buf[512];

    xv_set(fc, FRAME_BUSY, TRUE, 0);

    xv_set(ui->textsw,
	   TEXTSW_STATUS, &status,
	   TEXTSW_FILE,	path,
	   TEXTSW_FIRST, 0,
	   0);

    if ( status != TEXTSW_STATUS_OKAY ) {
	window_bell( ui->frame );
	xv_set( ui->frame,
	       FRAME_LEFT_FOOTER, "Unable to load file!",
	       0);
	xv_set(fc, FRAME_BUSY, FALSE, 0);
	return XV_ERROR;
    }

    /* Set current doc name on the Save popup. */
    (void) sprintf(buf, "%s.1", file);
    if ( ui->saveas )
	xv_set(ui->saveas, FILE_CHOOSER_DOC_NAME, buf, 0);
    else {
	if ( ui->doc_name )
	    free( ui->doc_name );
	ui->doc_name = strdup( buf );
    }

    (void) sprintf(buf, "Demo Text Editor - %s", file);
    xv_set(ui->frame,
	   XV_LABEL,	buf,
	   0);

    xv_set(fc, FRAME_BUSY, FALSE, 0);

    return XV_OK;
}




/*
 * Picked the Save button
 */
static int
my_save_callback(File_chooser fc, char * path, struct stat * stats)
{
    My_ui *ui = (My_ui *)xv_get(fc, XV_KEY_DATA, MY_KEY);
    char buf[512];

#ifdef FOR_REAL
    unsigned success;

    xv_set(fc, FRAME_BUSY, TRUE, 0);

    success = textsw_store_file(ui->textsw, path, 0, 0);

    if ( success != 0 ) {
	xv_set(ui->frame, FRAME_BUSY, FALSE, 0);
	return XV_ERROR;
    }

    /* created a new file, update the list */
    if ( !stats )
	xv_set(fc, FILE_CHOOSER_UPDATE, 0);

#endif /* FOR_REAL */

    (void) sprintf(buf, "Saved \"%s\"", path);
    xv_set(ui->frame,
	   FRAME_LEFT_FOOTER, buf,
	   FRAME_BUSY, FALSE,
	   0);

    return XV_OK;
}




static void
my_show_dot_files_proc(Panel_choice_item item, int value, Event * event)
{
    File_chooser fc = xv_get(item, XV_KEY_DATA, PARENT_FC_KEY);

    xv_set(fc, FILE_CHOOSER_SHOW_DOT_FILES, value, 0);
}


/*
 * FILE_CHOOSER_EXTEN_FUNC, layout extension items within the
 * given extension rect.
 */
static int
my_exten_func(File_chooser fc, Rect * frame_rect, Rect * exten_rect, int left_edge, int right_edge, int max_height)
{
    Panel_item item = (Panel_item) xv_get(fc, XV_KEY_DATA, EXTEN_ITEM_KEY);
    int item_width;

    item_width = (int) xv_get(item, XV_WIDTH);

    /*
     * show item centered in frame.
     */
    xv_set(item,
	   XV_X,	(frame_rect->r_width - item_width) / 2,
	   XV_Y,	exten_rect->r_top,
	   PANEL_PAINT, PANEL_NONE,
	   0);

    return -1;
}
