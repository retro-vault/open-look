
/*
 * Demonstrate the use of the PANEL_LIST_ROW_VALUES attribute
 */

#include <stdio.h>
#include <xview/xview.h>
#include <xview/font.h>
#include <xview/panel.h>


static Attr_attribute MY_KEY;

static void	my_clear_proc();
static void	my_load_proc();
static void	my_print_proc();

typedef struct {
    Frame frame;
    Panel_list_item list;
    Xv_font font;
} My_ui;


int
main(int argc, char ** argv)
{
    Panel panel;
    My_ui ui;

    (void) xv_init(XV_INIT_ARGC_PTR_ARGV, &argc, argv, 0);
    MY_KEY = xv_unique_key();

    ui.frame = xv_create ( XV_NULL, FRAME, 
			  XV_LABEL,		"New Load",
			  FRAME_SHOW_FOOTER,	TRUE,
			  0 );
    panel = xv_create ( ui.frame, PANEL, 0 );


    (void) xv_create ( panel, PANEL_BUTTON,
		      PANEL_LABEL_STRING,		"Load",
		      PANEL_NOTIFY_PROC,		my_load_proc,
		      XV_KEY_DATA,			MY_KEY, &ui,
		      0 );

    (void) xv_create ( panel, PANEL_BUTTON,
		      PANEL_LABEL_STRING,		"Clear",
		      PANEL_NOTIFY_PROC,		my_clear_proc,
		      XV_KEY_DATA,			MY_KEY, &ui,
		      0 );

    (void) xv_create( panel, PANEL_BUTTON,
		      PANEL_LABEL_STRING,		"Print Selected Row",
		      PANEL_NOTIFY_PROC,		my_print_proc,
		      XV_KEY_DATA,			MY_KEY, &ui,
		      0 );

    ui.list = xv_create( panel, PANEL_LIST,
			PANEL_LIST_WIDTH,		300,
			PANEL_LIST_DISPLAY_ROWS,	8,
			0);

    ui.font = xv_create(XV_NULL, FONT,
			FONT_FAMILY,	FONT_FAMILY_DEFAULT,
			FONT_STYLE,	FONT_STYLE_BOLD,
			0);

    window_fit ( panel );
    window_fit ( ui.frame );
    xv_main_loop ( ui.frame );
}


static void
my_clear_proc(Panel_item item, Event * event)
{
    My_ui *ui = (My_ui *)xv_get(item, XV_KEY_DATA, MY_KEY);
    int rows = (int)xv_get(ui->list, PANEL_LIST_NROWS);

    if ( rows > 0 )
	xv_set(ui->list,
	       PANEL_LIST_DELETE_ROWS,	0, rows,
	       0);
    xv_set(ui->frame, FRAME_LEFT_FOOTER, "", 0);
}


static void
my_load_proc(Panel_item item, Event * event)
{
    My_ui *ui = (My_ui *)xv_get(item, XV_KEY_DATA, MY_KEY);
    int ii;
    Panel_list_row_values vals[1000];
    char buf[64];

    xv_set(ui->frame, FRAME_BUSY, TRUE, 0);

    for(ii=0; ii<1000; ++ii) {
	vals[ii].string = "Testing PANEL_LIST_ROW_VALUES";
	vals[ii].font = ui->font;
	vals[ii].glyph = XV_NULL;
	vals[ii].mask_glyph = XV_NULL;
	vals[ii].client_data = ui->font;
	vals[ii].selected = FALSE;
	vals[ii].inactive = FALSE;
    }

    xv_set(ui->list,
	   PANEL_LIST_ROW_VALUES,	0, &vals, 1000,
	   0);

    (void) sprintf(buf, "%d rows in list", 
		   (int) xv_get(ui->list, PANEL_LIST_NROWS)
		   );
    xv_set(ui->frame,
	   FRAME_LEFT_FOOTER,	buf,
	   FRAME_BUSY, FALSE, 
	   0);
}



static void
my_print_proc(Panel_item item, Event * event)
{
    My_ui *ui = (My_ui *)xv_get(item, XV_KEY_DATA, MY_KEY);
    Panel_list_row_values vals;
    int row = (int) xv_get(ui->list, PANEL_LIST_FIRST_SELECTED);
    int count;

    count = (int) xv_get(ui->list, PANEL_LIST_ROW_VALUES, row, &vals, 1);

    if ( count != 1 ) {
	window_bell( ui->frame );
	xv_set(ui->frame, 
	       FRAME_LEFT_FOOTER,	"Unable to get row",
	       FRAME_BUSY, 		FALSE,
	       0);
	return;
    }

    printf( "Row Number %d:\n", row );
    printf( "\tString: %s\n", vals.string );
    printf( "\tSelected: %d\n", vals.selected );
    printf( "\tInactive: %d\n", vals.inactive );
}

