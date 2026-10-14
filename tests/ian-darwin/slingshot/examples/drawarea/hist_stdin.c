#include <xview/xview.h>
#include <sspkg/drawobj.h>
#include <sspkg/disp_list.h>
/*
 *	(c) Copyright 1991 Sun Microsystems, Inc.  All rights reserved.
 *	See LEGAL_NOTICE file for terms of the license.
 *
 *	@(#) hist_stdin.c 1.2 92/09/08 
 */

#include <xview/frame.h>
#include <xview/font.h>
#include <sspkg/canshell.h> 
#include <sspkg/box.h> 
#include <sspkg/rectobj.h> 
#include <sspkg/drawobj.h> 
 
void		resize();

Drawarea	hist;

/* input format:
 *	int+
 * e.g.:
 *	123 129 140 150 144 120 110
 *	107 112 130 133 122 112 98
 */


int read_input(void);

int main(int argc, char	* argv[])
{
	Frame		frame;
	Canvas_shell	shell;
	Box		tmpbox;
	Xv_Font		labelfont;
	Xv_Font		labelfont2;
	int		i;
	double left =	0.0;
	double right=	1.0;
	double upper = 1.0;
	double lower = 0.0;

	xv_init(XV_INIT_ARGC_PTR_ARGV, &argc, argv, 0);

	frame = (Frame) xv_create(XV_NULL, FRAME, 
		FRAME_LABEL, argv[0],
		XV_WIDTH, 800,
		XV_HEIGHT, 400,
		0);
 
/*
	labelfont = xv_find(0, FONT,
		FONT_NAME, "helvetica-24",
		0);

	labelfont2 = xv_find(0, FONT,
		FONT_NAME, "helvetica-18",
		0);
*/

	shell = (Canvas_shell) xv_create(frame, CANVAS_SHELL, 
		XV_WIDTH, 450,
		XV_HEIGHT, 200,
		0);

	hist = (Drawarea) xv_create(shell, DRAWAREA,
		RECTOBJ_RESIZABLE, TRUE,
		DRAWAREA_LEFT_X, &left,
		DRAWAREA_RIGHT_X, &right,
		DRAWAREA_UPPER_Y, &upper,
		DRAWAREA_LOWER_Y, &lower,
		XV_WIDTH, xv_get(shell, XV_WIDTH),
		XV_HEIGHT, xv_get(shell, XV_HEIGHT),
		0);

	xv_set(shell,
		CANVAS_RESIZE_PROC, resize,
		0);

	read_input();

	window_fit(frame);
	xv_main_loop(frame); 
} 

 
int read_input(void)
{
	int count = 0;
	double top = 0;
	double right = 0;
	double num;
	char string[1024];

	VClear(hist);

	while(scanf("%s", string) == 1) {
		num = (double) atoi(string);
		if(num > top)
			top = num;

		DFillRectangle(hist, .0 + count, 0.0, 1.0, num);
		count++;
	}

	right = (double) count;
	xv_set(hist,
		DRAWAREA_UPPER_Y, &top,
		DRAWAREA_RIGHT_X, &right,
		0);
}


void
resize(Canvas_shell shell, int width, int height)
{
	xv_set(hist,
		XV_WIDTH, width,
		XV_HEIGHT, height,
		0);
}

