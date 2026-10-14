/*
 *	(c) Copyright 1991 Sun Microsystems, Inc.  All rights reserved.
 *	See LEGAL_NOTICE file for terms of the license.
 *
 *	@(#) tlstool.c 1.7 92/10/23 
 */

#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <dirent.h>
#include <xview/frame.h>
#include <xview/openmenu.h>
#include <xview/scrollbar.h>
#include <xview/panel.h>
#include <sspkg/canshell.h> 
#include <sspkg/tree.h>
#include <sspkg/rectobj.h> 
#include <sspkg/drawobj.h> 


typedef struct {
	char	*full_name;
} Filetag;

#define FILETAG		XV_KEY_DATA, filetag_key
int	filetag_key;

Rectobj	add_leaf();
void	expand_subdirs();
void	set_horizontal();
void	set_vertical();
void	prune_proc();
Panel_setting	ptext_notify_proc();
void	resize();
void	dbl_click_proc();

Frame		frame;
Canvas_shell	shell;
Tree		tree = 0;

#define START_HEIGHT	300
#define START_WIDTH	500

int main(int argc, char	* argv[])
{
	Panel		panel;
	Panel_item	ptext;
	Panel_item	pbutton;
	Rectobj		root_node;
	char		*root_dir_name;
	Filetag		*filetag;
	Menu		menu;
	Scrollbar	vscroll;
	Scrollbar	hscroll;


	xv_init(XV_INIT_ARGC_PTR_ARGV, &argc, argv, 0);
 
	frame = (Frame) xv_create(XV_NULL, FRAME,
			FRAME_LABEL, argv[0],
			XV_WIDTH, START_WIDTH,
			XV_HEIGHT, START_HEIGHT,
			0);
 
	menu = (Menu) xv_create(XV_NULL, MENU_COMMAND_MENU,
			MENU_GEN_PIN_WINDOW, frame, "Options",
			MENU_ITEM,
				MENU_STRING, "Horizontal Layout",
				MENU_ACTION_PROC, set_horizontal,
				0,
			MENU_ITEM,
				MENU_STRING, "Vertical Layout",
				MENU_ACTION_PROC, set_vertical,
				0,
			MENU_ITEM,
				MENU_STRING, "Prune",
				MENU_ACTION_PROC, prune_proc,
				0,
			0);

	panel = (Panel) xv_create(frame, PANEL,
			PANEL_LAYOUT, PANEL_HORIZONTAL,
			0);

	pbutton = (Panel_item) xv_create(panel, PANEL_BUTTON,
			PANEL_LABEL_STRING, "Options",
			PANEL_ITEM_MENU, menu,
			0);

	ptext = (Panel_item) xv_create(panel, PANEL_TEXT,
			PANEL_LABEL_STRING, "Directory:",
			PANEL_VALUE, getenv("HOME"),
			PANEL_VALUE_DISPLAY_LENGTH, 30,
			PANEL_NOTIFY_PROC, ptext_notify_proc,
			0);

	window_fit(panel);
 
	shell = (Canvas_shell) xv_create(frame, CANVAS_SHELL, 
			XV_X, 0,
			XV_Y, 0,
			CANVAS_MIN_PAINT_WIDTH, START_WIDTH,
			CANVAS_MIN_PAINT_HEIGHT, START_HEIGHT,
			CANVAS_RESIZE_PROC, resize,
			RECTOBJ_MENU, menu,
			WIN_BELOW, panel,
			0);

	vscroll = (Scrollbar) xv_create(shell, SCROLLBAR,
			SCROLLBAR_DIRECTION, SCROLLBAR_VERTICAL,
			SCROLLBAR_SPLITTABLE, TRUE,
			SCROLLBAR_PIXELS_PER_UNIT, 10,
			0);
 
	hscroll = (Scrollbar) xv_create(shell, SCROLLBAR,
			SCROLLBAR_DIRECTION, SCROLLBAR_HORIZONTAL,
			SCROLLBAR_SPLITTABLE, TRUE,
			SCROLLBAR_PIXELS_PER_UNIT, 10,
			0);

	tree = (Tree) xv_create(shell, TREE,
			TREE_PARENT_DISTANCE, 50,
			XV_WIDTH, START_WIDTH,
			XV_HEIGHT, START_HEIGHT,
			0);

	window_fit(frame);
	xv_set(panel, 
		XV_WIDTH, WIN_EXTEND_TO_EDGE, 
		0);

	filetag_key = xv_unique_key();

	if(strlen(getenv("HOME")) == 1)
		root_dir_name = getenv("HOME");
	else
		root_dir_name = strrchr(getenv("HOME"), '/') + 1;

	(void) add_leaf(root_dir_name, getenv("HOME"), tree, tree);

	xv_main_loop(frame); 
} 
 


Rectobj
add_leaf(char		* filename, char		* full_name, Tree tree, Rectobj parent)
{
	Rectobj		new;
	Filetag		*filetag;


	filetag = (Filetag*) calloc(1, (unsigned) sizeof(Filetag));
	filetag->full_name = strdup(full_name);

	new = (Rectobj) xv_create(tree, DRAWTEXT, 
		DRAWTEXT_STRING, filename,
		RECTOBJ_DBL_CLICK_PROC, dbl_click_proc,
		FILETAG, filetag,
		0);

	xv_set(tree, 
		TREE_ADD_LINK, parent, new, 
		0);

	return new;
}


void
dbl_click_proc(Xv_window paint_window, Event		* event, Canvas_shell canvas_shell, Rectobj rectobj)
{
	Rectobj_list	*list;
	Rectobj		selected;

	xv_set(frame,
		FRAME_BUSY, TRUE,
		0);

	list = rectobj_get_selected_list();
	list_for(list) {
	  selected = RECTOBJ_LIST_HANDLE(list);
	  expand_subdirs(selected);
	}

	xv_set(frame,
		FRAME_BUSY, FALSE,
		0);
}


void
expand_subdirs(Rectobj rectobj)
{
	Filetag		*filetag;
	DIR		*dir;
	struct dirent	*dirent;
	char		*files[1000];
	int		i;
	int		j;
	char		full_filename[1024 /*MAXPATHLEN*/ ];

	filetag = (Filetag*) xv_get(rectobj, FILETAG);
	if(!filetag)
		return;
	dir = opendir(filetag->full_name);
	if(!dir)
		return;
	i=0;
	while(dirent = readdir(dir)) {
		if(dirent->d_name[0] == '.')
			continue;
		/*printf("> %s\n", dirent->d_name);*/
		files[i] = strdup(dirent->d_name);
		i++;
	}
	closedir(dir);
	if(i == 0)
		return;
	qsort(files, i, sizeof(char*), (int(*)())strcmp);

	xv_set(tree,
		XV_WIDTH, xv_get(shell, XV_WIDTH),
		XV_HEIGHT, xv_get(shell, XV_HEIGHT),
		0);

	for(j=0; j<i; j++) {
		/*printf("*> %s\n", files[j]);*/
		strcpy(full_filename, filetag->full_name);
		strcat(full_filename, "/");
		strcat(full_filename, files[j]);
		(void) add_leaf( files[j], full_filename, tree, rectobj);
		free(files[j]);
	}

	xv_set(shell,
		CANVAS_MIN_PAINT_WIDTH, xv_get(tree, XV_WIDTH),
		CANVAS_MIN_PAINT_HEIGHT, xv_get(tree, XV_HEIGHT),
		0);
}

 
void
set_horizontal(Menu menu, Menu_item menu_item)
{
	/*
	 * Set the size of the tree before changing it.
	 * This will have the effect of centering it if
	 * all the space isn't used.
	 */
	xv_set(tree,
		XV_WIDTH, xv_get(shell, XV_WIDTH),
		XV_HEIGHT, xv_get(shell, XV_HEIGHT),
		TREE_LAYOUT, TREE_LAYOUT_HORIZONTAL,
		0);

	/*
	 * In case the tree grew bigger than the paint window,
	 * set the paint window size.  This way, the scrollbars
	 * will be properly set.
	 */
	xv_set(shell,
		CANVAS_MIN_PAINT_WIDTH, xv_get(tree, XV_WIDTH),
		CANVAS_MIN_PAINT_HEIGHT, xv_get(tree, XV_HEIGHT),
		0);
}
                 
 
void
set_vertical(Menu menu, Menu_item menu_item)
{
	xv_set(tree,
		XV_WIDTH, xv_get(shell, XV_WIDTH),
		XV_HEIGHT, xv_get(shell, XV_HEIGHT),
		TREE_LAYOUT, TREE_LAYOUT_VERTICAL,
		0);

	xv_set(shell,
		CANVAS_MIN_PAINT_WIDTH, xv_get(tree, XV_WIDTH),
		CANVAS_MIN_PAINT_HEIGHT, xv_get(tree, XV_HEIGHT),
		0);
}
 
 
 
void
prune_proc(Menu menu, Menu_item menu_item)
{
	Rectobj_list	*list;


	xv_set(shell, 
		CANVAS_SHELL_DELAY_REPAINT, TRUE, 
		0);

	xv_set(tree,
		XV_WIDTH, xv_get(shell, XV_WIDTH),
		XV_HEIGHT, xv_get(shell, XV_HEIGHT),
		0);

	/*
	 * destroy the selected nodes...
	 * Have to be careful with the list because they list is being
	 * altered as we traverse it.
	 */
	while( list = rectobj_get_selected_list() )
		xv_destroy(RECTOBJ_LIST_HANDLE(list));

	xv_set(shell, 
		CANVAS_MIN_PAINT_WIDTH, xv_get(tree, XV_WIDTH),
		CANVAS_MIN_PAINT_HEIGHT, xv_get(tree, XV_HEIGHT),
		CANVAS_SHELL_DELAY_REPAINT, FALSE, 
		0);

}
 

Panel_setting
ptext_notify_proc(Panel_item item, Event		* event)
{

	Rectobj_list	*list;
	char		*new_path;
	char		*directory;

	new_path = (char*) xv_get(item, PANEL_VALUE);

	if((!new_path) || (!*new_path))
		return 0;

	directory = strrchr(new_path, '/');

	if(!directory)
		directory = new_path;
	else
		directory++;

	xv_set(frame,
		FRAME_BUSY, TRUE,
		0);
	xv_set(shell, 
		CANVAS_SHELL_DELAY_REPAINT, TRUE, 
		0);

	/* destroy the current nodes in the tree */
	list = (Rectobj_list*)xv_get(tree, TREE_LINK_TO_LIST, tree);
	list_for(list)
		xv_destroy(RECTOBJ_LIST_HANDLE(list));

	expand_subdirs( add_leaf(directory, new_path, tree, tree) );

	xv_set(frame,
		FRAME_BUSY, FALSE,
		0);
	xv_set(shell, 
		CANVAS_SHELL_DELAY_REPAINT, FALSE, 
		0);

}



void
resize(Canvas_shell shell, int width, int height)
{
	if(tree)
		xv_set(tree, 
			XV_WIDTH, width,
			XV_HEIGHT, height,
			0);
}

