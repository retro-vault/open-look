/* Extracted from xview.multi-frame; see adjacent archived message for author. */
/*
	cc <file.c> -o <file> -I$OPENWINHOME/include -L$OPENWINHOME/lib \
			-lxview -lolgx -lX11
*/
#include <stdio.h>
#include <xview/xview.h>
#include <xview/panel.h>

Frame	   frame1, frame;
Panel	   panel1, panel;
Panel_item text1,  text;

int main(argc,argv)
int	argc;
char*	argv[];
{

	Xv_Server	server1;
	Xv_Screen	screen;
	Xv_Window	root1;

	xv_init(XV_INIT_ARGS,argc,argv,NULL);

	server1 = xv_create(NULL, SERVER,
				XV_NAME, 	"hostabc:0", 
				NULL);

	screen = (Xv_Screen)xv_get(server1, SERVER_NTH_SCREEN, 0);
	root1 = (Xv_Window)xv_get(screen, XV_ROOT);


	frame1 = xv_create(root1, FRAME, NULL);
	panel1 = xv_create(frame1, PANEL, NULL);

	frame = xv_create(NULL, FRAME, NULL);
	panel = xv_create(frame, PANEL, NULL);

	text1 = xv_create(panel1, PANEL_TEXT, NULL);

	text = xv_create(panel, PANEL_TEXT, NULL);

	xv_set(frame1, WIN_SHOW, TRUE, NULL);

	xv_main_loop(frame);
}






