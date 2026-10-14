/* (C) Universitaet Passau 1986-1991 */
/* GraphEd Source, 1986-1991 by Michael Himsolt	*/
/************************************************************************/
/*									*/
/*				canvas.c				*/
/*									*/
/************************************************************************/
/*									*/
/*	Dieses Modul verwaltet die Arbeitsflaeche (working_area_canvas)	*/
/*									*/
/************************************************************************/

#include "misc.h"
#include "graph.h"
#include "graphed_subwindows.h"
#include "graphed_mpr.h"

#include "user.h"
#include "repaint.h"

#include "menu.h"


/************************************************************************/
/*									*/
/*			GLOBALE FUNKTIONEN				*/
/*									*/
/************************************************************************/
/*									*/
/*	void	create_canvas (n, canvas_width, canvas_height)		*/
/*									*/
/*	void	set_working_area_canvas       (canvas)			*/
/*	void	set_wac_mouse_position        (x,y)			*/
/*	void	set_working_area_size         (width, height)		*/
/*									*/
/*	void	set_canvas_window             (n, x,y, width, height)	*/
/*	void	set_canvas_window_size        (n, width, height)	*/
/*									*/
/*	void	scroll_working_area           (x,y)			*/
/*	void	scroll_working_area_relative  (dx,dy)			*/
/*	void	scroll_working_area_to_middle (dx,dy)			*/
/*									*/
/*	void	get_scroll_offset (buffer, offset_x, offset_y)		*/
/*	void	translate_wac_to_base_frame_space (x,y)			*/
/*									*/
/************************************************************************/


static	Notify_value	canvas_destroyer    ();
static	void		set_canvas_colormap ();

/************************************************************************/
/*									*/
/*			GLOBALE VARIABLEN				*/
/*									*/
/************************************************************************/


/***************	working_area_canvas (wac)	*****************/

Graphed_canvas	canvases [N_BUFFERS];

Canvas	working_area_canvas;		/* Aktuelle Arbeitsflaeche	*/


/************************************************************************/
/*									*/
/*			LOKALE VARIABLEN				*/
/*									*/
/************************************************************************/


u_char	red   [GRAPHED_COLORMAPSIZE],
	green [GRAPHED_COLORMAPSIZE],
	blue  [GRAPHED_COLORMAPSIZE];


/************************************************************************/
/*									*/
/*				CANVAS AUFBAUEN				*/
/*									*/
/************************************************************************/
/*									*/
/*	void	create_canvas (n, canvas_width, canvas_height)		*/
/*									*/
/*	Createsa canvas no. n.						*/
/*	XV_SHOW remains FALSE.						*/
/*									*/
/*======================================================================*/
/*									*/
/*	void	set_canvas_window      (n, x,y, width, height)		*/
/*	void	set_canvas_window_size (n, width, height)		*/
/*									*/
/*	Set size and position (resp. only size) of canvases[n].		*/
/*	This procedure does not change the canvas, but the frame !	*/
/*	XV_SHOW is set to TRUE.					*/
/*									*/
/*======================================================================*/
/*									*/
/*	show_grid (draw.c) and set_filename (load.c) also modify the	*/
/*	'canvases'-structure !						*/
/*									*/
/************************************************************************/


void	init_canvases(void)
{
	int	i;
	
	for (i=0; i<N_BUFFERS; i++) {
		canvases[i].frame  = (Frame)0;
		canvases[i].canvas = (Canvas)0;
		canvases[i].horizontal_scrollbar = (Scrollbar)0;
		canvases[i].vertical_scrollbar   = (Scrollbar)0;
		canvases[i].gridwidth = 0;
	}
}


static short graphed_canvas_icon_data[] =
{
#include "images/graphed_canvas.icon"
};
mpr_static (graphed_canvas_icon_pixrect,
            DEFAULT_ICON_WIDTH, DEFAULT_ICON_HEIGHT, DEFAULT_ICON_DEPTH,
            graphed_canvas_icon_data);


static	canvas_frame_done_proc(Frame frame)
{
	xv_destroy_safe (frame);
}


int	create_canvas(int n, int canvas_width, int canvas_height)
{
static	Panel_item	panel_create_menu_button,
			panel_edit_menu_button,
			panel_gragra_menu_button,
			panel_file_menu_button,
			panel_misc_menu_button,
			panel_tools_menu_button,
			panel_layout_menu_button,
			panel_goodies_menu_button,
			panel_user_menu_button,
			panel_about_menu_button;

	/*	Create a frame to contain the canvas			*/
	
	canvases[n].frame = (Frame)xv_create(base_frame, FRAME,
		FRAME_ICON,		icon_create(ICON_IMAGE,	pr_to_svi(&graphed_canvas_icon_pixrect), 0),
/*		FRAME_DONE_PROC,	canvas_destroyer, */
		FRAME_DONE_PROC,	canvas_frame_done_proc,
		XV_WIDTH,		canvas_width,
		XV_HEIGHT,		canvas_height,
		0);

	menubar_panel = (Panel)xv_create(canvases[n].frame, PANEL,
		PANEL_LAYOUT,		PANEL_HORIZONTAL,
		WIN_ERROR_MSG,		"Could not create menubar panel.\nGood bye!\n",
		XV_HEIGHT,		25,
		0);
	panel_create_menu_button = xv_create(menubar_panel, PANEL_BUTTON,
		PANEL_LABEL_STRING,     "Create",
		PANEL_ITEM_MENU,	create_submenu,
		0);
	panel_edit_menu_button = xv_create(menubar_panel, PANEL_BUTTON,
		PANEL_LABEL_STRING,	"Edit",
		PANEL_ITEM_MENU,	edit_submenu,
		0);
	panel_gragra_menu_button = xv_create(menubar_panel, PANEL_BUTTON,
		PANEL_LABEL_STRING,     "GraGra",
		PANEL_ITEM_MENU,	gragra_submenu,
		0);
	panel_file_menu_button = xv_create(menubar_panel, PANEL_BUTTON,
		PANEL_LABEL_STRING,	"File",
		PANEL_ITEM_MENU,	file_submenu,
		0);
	panel_misc_menu_button = xv_create(menubar_panel, PANEL_BUTTON,
		PANEL_LABEL_STRING,	"Misc",
		PANEL_ITEM_MENU,	misc_submenu,
		0);
	panel_tools_menu_button = xv_create(menubar_panel, PANEL_BUTTON,
		PANEL_LABEL_STRING,	"Tools",
		PANEL_ITEM_MENU,	tools_submenu,
		0);
	panel_layout_menu_button = xv_create(menubar_panel, PANEL_BUTTON,
		PANEL_LABEL_STRING,	"Layout",
		PANEL_ITEM_MENU,	layout_submenu,
		0);
	panel_goodies_menu_button = xv_create(menubar_panel, PANEL_BUTTON,
		PANEL_LABEL_STRING,	"Goodies",
		PANEL_ITEM_MENU,	goodies_submenu,
		0);
	panel_user_menu_button = xv_create(menubar_panel, PANEL_BUTTON,
		PANEL_LABEL_STRING,	"User",
		PANEL_ITEM_MENU,	user_submenu,
		0);
	panel_about_menu_button = xv_create(menubar_panel, PANEL_BUTTON,
		PANEL_LABEL_STRING,	"Help",
		PANEL_ITEM_MENU,	about_submenu,
		0);

	canvases[n].canvas = (Canvas)xv_create(canvases[n].frame, CANVAS,
		WIN_X,			0,
		WIN_BELOW,		menubar_panel,
		CANVAS_RETAINED,        FALSE,
		CANVAS_AUTO_SHRINK,	FALSE,
		CANVAS_AUTO_EXPAND,	FALSE,
		OPENWIN_AUTO_CLEAR,	FALSE,
		CANVAS_FIXED_IMAGE,	FALSE,
/*		CANVAS_WIDTH,		canvas_width,
		CANVAS_HEIGHT,		canvas_height,*/
		CANVAS_REPAINT_PROC,	repaint_canvas,
		0);

	canvases[n].horizontal_scrollbar = (Scrollbar)xv_create(canvases[n].canvas, SCROLLBAR,
		SCROLLBAR_DIRECTION,	SCROLLBAR_HORIZONTAL,
		SCROLLBAR_SPLITTABLE,	FALSE,
		0);
	canvases[n].vertical_scrollbar = (Scrollbar)xv_create(canvases[n].canvas, SCROLLBAR,
		SCROLLBAR_DIRECTION,	SCROLLBAR_VERTICAL,
		SCROLLBAR_SPLITTABLE,	FALSE,
		0);

	xv_set(canvas_paint_window(canvases[n].canvas),
		WIN_EVENT_PROC,		working_area_event_proc,
		WIN_CONSUME_EVENTS,	KBD_DONE, KBD_USE,
					LOC_DRAG, LOC_MOVE,
					LOC_WINENTER, LOC_WINEXIT,
					WIN_ASCII_EVENTS, WIN_META_EVENTS,
					WIN_MOUSE_BUTTONS,
					0,
		0);

	canvases[n].pixwin = canvas_pixwin (canvases[n].canvas);

	set_canvas_colormap(canvases[n].canvas);
	
	set_canvas_frame_label(n);
	notify_interpose_destroy_func(canvases[n].frame, canvas_destroyer);

	return	TRUE;
}


static	Notify_value	canvas_destroyer(Notify_client frame, Destroy_status status)
{
	Prompt	user_choice;
	int	i;
	int	frame_still_exists;

	for (i=0; i<N_BUFFERS; i++)
		if (canvases[i].frame == frame)
			break;
	frame_still_exists = (i<N_BUFFERS);
	
	if (status == DESTROY_CHECKING && !graphed_state.shutdown) {
		
		dispatch_user_action (UNSELECT);

		if (user_interface_check_destroy_buffer (i) == FALSE) {
			(void)notify_veto_destroy(frame);
			bell ();
			return NOTIFY_DONE;
		} else if (buffers[i].changed) {
			user_choice = notice_prompt (frame, 0,	/*fisprompt*/
				NOTICE_FOCUS_XY,	screenwidth/3, screenheight/2,
				NOTICE_MESSAGE_STRINGS,	"Some graphs have changed since last save.",
							"really quit ?", 0,
				NOTICE_BUTTON,		"yes",		PROMPT_ACCEPT,
				NOTICE_BUTTON,		"no",		PROMPT_REFUSE,
				NOTICE_BUTTON,		"cancel",	PROMPT_CANCEL,
				0);
			switch (user_choice) {
			    case PROMPT_ACCEPT :
				break;
			    case PROMPT_REFUSE :
			    case PROMPT_CANCEL :
				(void)notify_veto_destroy(frame);
				return NOTIFY_DONE;
				break;
			}
		}
		
		xv_set(frame, FRAME_NO_CONFIRM, TRUE, 0);

		if (frame_still_exists) {
			/* Added MH Conversion			    */
			/* because frame might be already destroyed */
			delete_graphs_in_buffer (i);
		}

		unuse_buffer (i); /* cannot call delete_buffer because	*/
				  /* this proc calls xv_destroy_safe !	*/
		canvases[i].frame  = (Frame)0;
		canvases[i].canvas = (Canvas)0;
		canvases[i].horizontal_scrollbar = (Scrollbar)0;
		canvases[i].vertical_scrollbar   = (Scrollbar)0;
		canvases[i].pixwin               = (Pixwin *)0;
		canvases[i].gridwidth            = 0;
	
	} else {
	
		/* skip	*/
	}
	
	return(notify_next_destroy_func(frame,status));
}



void	destroy_frame_and_canvas(int n)
{
	if (canvases[n].frame != (Frame)0) {
		xv_destroy_safe(canvases[n].frame);
	
		canvases[n].frame  = (Frame)0;
		canvases[n].canvas = (Canvas)0;
		canvases[n].horizontal_scrollbar = (Scrollbar)0;
		canvases[n].vertical_scrollbar   = (Scrollbar)0;
		canvases[n].pixwin               = (Pixwin *)0;
		canvases[n].gridwidth = 0;
	}
}




void	set_canvas_frame_label(int n)
{
	int	edited;
	char	filename [FILENAMESIZE];
	char	buffer   [FILENAMESIZE];
	char	icon_buffer [FILENAMESIZE];
		
	
	edited = buffers[n].changed;
	
	if (buffers[n].filename == 0 || !strcmp(buffers[n].filename, "")) {
		strcpy (filename, "(None)");
	} else {
		strncpy (filename, buffers[n].filename, FILENAMESIZE);
	}
	
	sprintf (buffer, "%s FILE : %s %s",
		iif (n == wac_buffer, "*", " "),
		filename,
		iif (edited, "(edited)", ""));
	
	sprintf (icon_buffer, "%s%s", iif (edited, ">", ""), filename);
	
	xv_set(canvases[n].frame, XV_LABEL, buffer, 0);

	icon_set (xv_get(canvases[n].frame, FRAME_ICON),
		XV_LABEL, strsave (icon_buffer),
		0);
}



void	set_canvas_window_size(int n, int width, int height)
{
	xv_set(canvases[n].frame,
		XV_WIDTH,	width,
		XV_HEIGHT,	height,
		0);

	if ((int)xv_get (canvases[n].frame, XV_SHOW) == FALSE) {
		xv_set (canvases[n].frame,
			XV_SHOW,   TRUE,
			0);
	}
}




void	set_canvas_window(int n, int x, int y, int width, int height)
{
	xv_set(canvases[n].frame,
		WIN_X,		x,
		WIN_Y,		y,
		XV_WIDTH,	width,
		XV_HEIGHT,	height,
		0);

	if ((int)xv_get (canvases[n].frame, XV_SHOW) == FALSE) {
		xv_set (canvases[n].frame,
			XV_SHOW,   TRUE,
			0);
	}
}



void	set_canvas_window_silent(int n, int x, int y, int width, int height)
{
	xv_set(canvases[n].frame,
		WIN_X,		x,
		WIN_Y,		y,
		XV_WIDTH,	width,
		XV_HEIGHT,	height,
		0);
}
/************************************************************************/
/*									*/
/*			COLORMAP					*/
/*									*/
/************************************************************************/
/*									*/
/*	void	init_graphed_colormap ()				*/
/*									*/
/*======================================================================*/
/*									*/
/*	static	void	set_canvas_colormap (canvas)			*/
/*									*/
/************************************************************************/


void	init_graphed_colormap(void)
{
#define set_color(i,r,g,b)	\
	red  [(i)] = (r);	\
	green[(i)] = (g);	\
	blue [(i)] = (b);
	
	/* MUST BE CONSISTENT WITH THE CONSTANTS BLACK AND WHITE	*/
	/* DECLARED IN PAINT.H						*/
	
	set_color (0,  255, 255, 255);
	set_color (1,  255, 0,   0);
	set_color (2,  0,   255, 0);
	set_color (3,  0,   0,   255);
	set_color (4,  0,   255, 255);
	set_color (5,  255, 255, 0);
	set_color (6,  255, 0,   255);
	set_color (7,  0,   0,   0);
	
	set_color (8,  192, 192, 192);
	set_color (9,  192, 0,   0);
	set_color (10, 0,   192, 0);
	set_color (11, 0,   0,   192);
	set_color (12, 0,   192, 192);
	set_color (13, 192, 192, 0);
	set_color (14, 192, 0,   192);
	set_color (15, 0,   0,   0);
}


static	void	set_canvas_colormap(Canvas canvas)
{
	Pixwin	*pw;
	
/* Commented out MH Conversion

	pw = canvas_pixwin (canvas);
	
	pw_setcmsname  (pw, GRAPHED_COLORMAP_NAME);
	pw_putcolormap (pw, 0, GRAPHED_COLORMAPSIZE, red, green, blue);
	
	xv_set(canvas,
		WIN_VERTICAL_SCROLLBAR,		(Scrollbar)xv_get(canvas, WIN_VERTICAL_SCROLLBAR),
		WIN_HORIZONTAL_SCROLLBAR,	(Scrollbar)xv_get(canvas, WIN_HORIZONTAL_SCROLLBAR),
		0);
*/
}
/************************************************************************/
/*									*/
/*			VERWALTUNGSPROZEDUREN				*/
/*									*/
/************************************************************************/
/*									*/
/*	void	set_working_area_canvas (canvas)			*/
/*									*/
/*	Setzt canvas als aktuelle working area. Eine Anzahl globaler	*/
/*	Variablen, die dazu gehoeren, werden ebenfalls umgesetzt.	*/
/*									*/
/*======================================================================*/
/*									*/
/*	void	set_wac_mouse_position (x,y)				*/
/*									*/
/*	Setzt die Cursorposition auf der working_area.			*/
/*									*/
/*======================================================================*/
/*									*/
/*	int	set_working_area_size (width, height)			*/
/*									*/
/*	Setzt die Groesse des working_area_canvas neu. Falls die neue	*/
/*	Groesse nicht gesetzt werden kann, da der Graph zu gross ist,	*/
/*	wird FALSE zurueckgegeben, sonst TRUE.				*/
/*									*/
/*======================================================================*/
/*									*/
/*	void	scroll_working_area           (x,y)			*/
/*	(x,y) sind linke obere Ecke des Fensters.			*/
/*									*/
/*	void	scroll_working_area_relative  (dx,dy)			*/
/*									*/
/*	void	scroll_working_area_to_middle ()			*/
/*									*/
/************************************************************************/


void	set_working_area_canvas(Canvas canvas)
{
	int	i,
		old_wac_buffer = wac_buffer;
	
	/* a call to force_repainting is necessary to repaint the	*/
	/* previous canvas ... but only if it is used			*/
	
	if (buffers[wac_buffer].used)
		force_repainting ();
	
	working_area_canvas = canvas;
	
	for (i=0; i<N_BUFFERS; i++) {
		if (canvases[i].canvas == canvas)
			break;
	}
	wac_buffer = i;
	
	if (old_wac_buffer >= N_PASTE_BUFFERS)
		set_canvas_frame_label (old_wac_buffer);
	set_canvas_frame_label (wac_buffer);
}


void	set_mouse_position(int buffer, int x, int y)
{
	int	scroll_offset_x, scroll_offset_y;
	
	get_scroll_offset (buffer, &scroll_offset_x, &scroll_offset_y);
	xv_set(canvases[buffer].canvas, WIN_MOUSE_XY, x - scroll_offset_x, y - scroll_offset_y, 0);
}


void	set_wac_mouse_position(int x, int y)
{
	set_mouse_position (wac_buffer, x,y);
}



int	set_buffer_size(int buffer, int width, int height)
{
	Rect	graphs_rect;
	
	graphs_rect = compute_rect_around_graphs (buffer);
	
	if (width  < rect_right(&graphs_rect) ||
	    height < rect_bottom(&graphs_rect)) {
		warning ("Cannot make buffer smaller.\n");
		return FALSE;
	} 
	
	xv_set(canvases[buffer].canvas,
		CANVAS_WIDTH,	width,
		CANVAS_HEIGHT,	height,
		0);
		
	return TRUE;
}



int	set_working_area_size(int width, int height)
{
	return	set_buffer_size (wac_buffer, width, height);
}



void	scroll_buffer(int buffer, int x, int y)
{
	Rect	buffer_rect; /* Rechteck auf dem Bildschirm */
	
	pw_get_region_rect (canvases[wac_buffer].pixwin, &buffer_rect);
	x = minimum (x, (int)xv_get(canvases[buffer].canvas, CANVAS_WIDTH) -
	                rect_width(&buffer_rect));
	y = minimum (y, (int)xv_get(canvases[buffer].canvas, CANVAS_HEIGHT) -
	                rect_height(&buffer_rect));
	x = maximum (x,0);
	y = maximum (y,0);
	scrollbar_scroll_to (canvases[buffer].horizontal_scrollbar, (long)x);
	scrollbar_scroll_to (canvases[buffer].vertical_scrollbar,   (long)y);
}


void	scroll_working_area(int x, int y)
{
	scroll_buffer (wac_buffer, x,y);
}


void	scroll_working_area_relative(int dx, int dy)
{
	int	scroll_offset_x, scroll_offset_y;
	
	get_scroll_offset  (wac_buffer, &scroll_offset_x, &scroll_offset_y);
	scroll_working_area (scroll_offset_x + dx, scroll_offset_y + dy);
}


void	scroll_working_area_to_middle(void)
{
	scroll_working_area (
		(int)xv_get(working_area_canvas, CANVAS_WIDTH)/2
			- (int)xv_get(working_area_canvas, XV_WIDTH) / 2,
		(int)xv_get(working_area_canvas, CANVAS_HEIGHT)/2
			- (int)xv_get(working_area_canvas, XV_HEIGHT) / 2);
}


void	center_buffer_around(int buffer, int x, int y)
{
	int	win_width  = (int)xv_get(canvases[buffer].canvas, XV_WIDTH);
	int	win_height = (int)xv_get(canvases[buffer].canvas, XV_HEIGHT);
	
	scroll_buffer (buffer, x-win_width/2, y-win_height/2);
}


void	get_buffer_center(int buffer, int	* x, int	 *y)
{
	int	win_width  = (int)xv_get(canvases[buffer].canvas, XV_WIDTH);
	int	win_height = (int)xv_get(canvases[buffer].canvas, XV_HEIGHT);
	int	scroll_start_x, scroll_start_y;
	
	get_scroll_offset (buffer, &scroll_start_x, &scroll_start_y);
	
	*x = scroll_start_x + win_width/2;
	*y = scroll_start_y + win_height/2;
}


void	get_buffer_visible_rect(int buffer, Rect	* rect)
{
	int	win_width  = (int)xv_get(canvases[buffer].canvas, XV_WIDTH);
	int	win_height = (int)xv_get(canvases[buffer].canvas, XV_HEIGHT);
	int	scroll_start_x, scroll_start_y;
	
	get_scroll_offset (buffer, &scroll_start_x, &scroll_start_y);
	
	rect_construct (rect, scroll_start_x,
	                      scroll_start_y,
	                      scroll_start_x + win_width,
	                      scroll_start_y + win_height);	
}
/************************************************************************/
/*									*/
/*	    HILFSPROZEDUREN ZUR KOORNINATENTRANSFORMATION		*/
/*									*/
/************************************************************************/
/*									*/
/*	void	get_scroll_offset (buffer, offset_x,offset_y);		*/
/*									*/
/*	Gibt den Wert zurueck, um den der working_area_canvas gescrollt	*/
/*	ist (in Pixeln).						*/
/*									*/
/*======================================================================*/
/*									*/
/*	void	translate_wac_to_base_frame_space (x,y)			*/
/*									*/
/*	Koordinatenumrechnung. Diese Transformation wird z.B.		*/
/*	gebraucht, um einen Subframe anhand von Canvaskoordinaten	*/
/*	zu positionieren.						*/
/*									*/
/************************************************************************/


void	get_scroll_offset(int buffer, int	* offset_x, int	* offset_y)
{
	*offset_x = xv_get(canvases[buffer].horizontal_scrollbar, SCROLLBAR_VIEW_START);
	*offset_y = xv_get(canvases[buffer].vertical_scrollbar, SCROLLBAR_VIEW_START);
}


void	translate_wac_to_base_frame_space(int	* x, int	 *y)
{
	int		scrolled_x, scrolled_y;
	
	get_scroll_offset (wac_buffer, &scrolled_x, &scrolled_y);
	*x = *x - (scrolled_x + (int)xv_get(working_area_canvas, WIN_X))
	        + (int)xv_get(canvases[wac_buffer].canvas, XV_LEFT_MARGIN)
	        + (int)xv_get(canvases[wac_buffer].frame, WIN_X);
	*y = *y - (scrolled_y + (int)xv_get(working_area_canvas, WIN_Y))
	        + (int)xv_get(canvases[wac_buffer].canvas, XV_TOP_MARGIN)
	        + (int)xv_get(canvases[wac_buffer].frame, WIN_Y);

}
