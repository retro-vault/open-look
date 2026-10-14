/*
 *	(c) Copyright 1991 Sun Microsystems, Inc.  All rights reserved.
 *	See LEGAL_NOTICE file for terms of the license.
 *
 * 	@(#) example1.c 1.3 92/06/06 
 */

#include <xview/frame.h>
#include <sspkg/canshell.h> 
#include <sspkg/rectobj.h> 
#include <sspkg/drawobj.h> 
 
int main(int argc, char	* argv[])
{
	Frame		frame;
	Canvas_shell	shell;
	Rectobj		rectobj1;
	Drawrect 	drawrect1;
	Drawtext	drawtext1;

 
	xv_init(XV_INIT_ARGC_PTR_ARGV, &argc, argv, 0);
 
	frame = (Frame) xv_create(0, FRAME, 
			FRAME_LABEL, argv[0],
			0);
 
	shell = (Canvas_shell) xv_create(frame, CANVAS_SHELL, 0);
 
	rectobj1 = (Rectobj) xv_create(shell, BAG,
			XV_X, 100,
			0);

	drawrect1 = (Drawrect) xv_create(rectobj1, DRAWRECT, 
			XV_Y, 16,
			XV_WIDTH, 200,
			XV_HEIGHT, 200,
			0);

	drawtext1 = (Drawtext) xv_create(rectobj1, DRAWTEXT, 
			DRAWTEXT_STRING, "Hello World",
			XV_X, 60,
			XV_Y, 10,
			0);

	xv_main_loop(frame); 
} 
 

