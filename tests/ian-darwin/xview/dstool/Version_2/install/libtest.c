/*  -------------------------------------------------------------------

This program is the property of:

                             Cornell University 
                        Center for Applied Mathematics 
                              Ithaca, NY 14853

and may be used, modified and distributed freely, subject to the 
following restrictions:

       Any product which incorporates source code from the dstool
       program or utilities, in whole or in part, is distributed
       with a copy of that source code, including this notice. You
       must give the recipients all the rights that you have with
       respect to the use of this software. Modifications of the
       software must carry prominent notices stating who changed
       the files and the date of any change.

DsTool is distributed in the hope that it will be useful, but 
WITHOUT ANY WARRANTY; without even the implied warranty of FITNESS 
FOR A PARTICULAR PURPOSE.  The software is provided as is without 
any obligation on the part of Cornell faculty, staff or students to 
assist in its use, correction, modification or enhancement.

  -----------------------------------------------------------------  */

#include <stdio.h>
#include <sys/param.h>
#include <sys/types.h>
#include <xview/xview.h>
#include <xview/canvas.h>
#include <xview/icon_load.h>
#include <xview/panel.h>
#include <xview/scrollbar.h>
#include <xview/svrimage.h>
#include <xview/termsw.h>
#include <xview/text.h>
#include <xview/tty.h>
#include <xview/xv_xrect.h>
#include <xview/textsw.h>
#include <xview/font.h>
#include <gcm.h>
#include <gdd.h>



#ifndef	libtest_HEADER
#define	libtest_HEADER

extern Attr_attribute	INSTANCE;


typedef struct {
	Xv_opaque	base;
	Xv_opaque	canvas;
} libtest_base_objects;

extern libtest_base_objects	*libtest_base_objects_initialize();

extern Xv_opaque	libtest_base_base_create();
extern Xv_opaque	libtest_base_canvas_create();
#endif


Attr_attribute	INSTANCE;
libtest_base_objects	*libtest_base;

int
main(int argc, char		** argv)
{
	
	/*
	 * Initialize XView.
	 */
	xv_init(XV_INIT_ARGC_PTR_ARGV, &argc, argv, 0);
	INSTANCE = xv_unique_key();
	
	/*
	 * Initialize user interface components.
	 */
	libtest_base = libtest_base_objects_initialize(0, 0);
	
	/*
	 * Turn control over to XView.
	 */
	xv_main_loop(libtest_base->base);
	exit(0);
}



/*
 * Initialize an instance of object `base'.
 */
libtest_base_objects *
libtest_base_objects_initialize(libtest_base_objects	* ip, Xv_opaque owner)
{
	if (!ip && !(ip = (libtest_base_objects *) calloc(1, sizeof (libtest_base_objects))))
		return (libtest_base_objects *) 0;
	if (!ip->base)
		ip->base = libtest_base_base_create(ip, owner);
	if (!ip->canvas)
		ip->canvas = libtest_base_canvas_create(ip, ip->base);
	return ip;
}

/*
 * Create object `base' in the specified instance.
 */
Xv_opaque
libtest_base_base_create(caddr_t ip, Xv_opaque owner)
{
	Xv_opaque	obj;
	
	obj = xv_create(owner, FRAME,
		XV_KEY_DATA, INSTANCE, ip,
		XV_WIDTH, 475,
		XV_HEIGHT, 250,
		XV_LABEL, "Library Test Program",
		FRAME_CLOSED, FALSE,
		FRAME_SHOW_FOOTER, TRUE,
		FRAME_SHOW_RESIZE_CORNER, FALSE,
		0);
	return obj;
}

#define GC_KEY 10

/*
 * Create object `canvas' in the specified instance.
 */
Xv_opaque
libtest_base_canvas_create(caddr_t ip, Xv_opaque owner)
{
	Xv_opaque	obj;
	void		repaint_proc();
	
	obj = xv_create(owner, CANVAS,
		XV_KEY_DATA, INSTANCE, ip,
		XV_X, 0,
		XV_Y, 0,
		XV_WIDTH, WIN_EXTEND_TO_EDGE,
		XV_HEIGHT, WIN_EXTEND_TO_EDGE,
		CANVAS_X_PAINT_WINDOW, TRUE,
		CANVAS_REPAINT_PROC, repaint_proc, 
		0);
	gcm_initialize_colors(obj, "Light Steel Blue", 0);
	xv_set(canvas_paint_window(obj), XV_KEY_DATA, INSTANCE, ip, 0);


	return obj;
}

void
repaint_proc(Canvas canvas, Xv_Window pw, Display		* dpy, Window xwin, Xv_xrectlist	* xrects)
{
	Frame		frame = (Frame) libtest_base->base;
	XGCValues	gcvalues;
	Xv_Font		font;
	GC		gc1, gc2;

	int		i;
	static int	first = 0, left = 300, top = 200;
	Rect  		*rect;

	rect = (Rect *)calloc(1,sizeof(Rect));
	frame_get_rect(frame,rect); /* get the current configuration */
	rect->r_left = (short) left;
	rect->r_top = (short) top;
	frame_set_rect(frame,rect); /* set the current configuration */
	free(rect);

	font = (Xv_Font) xv_find(frame, FONT, 
				 FONT_FAMILY, FONT_FAMILY_ROMAN,
				 FONT_SIZE, 18,
				 FONT_STYLE, FONT_STYLE_BOLD_ITALIC, 0);
	if(!font)
	   font = (Xv_Font) xv_get(frame, XV_FONT);
	gcvalues.font = (Font) xv_get(font, XV_XID);
	gcvalues.foreground = BlackPixel(dpy, DefaultScreen(dpy));
	gcvalues.graphics_exposures = False;
	gc1 = XCreateGC(dpy, RootWindow(dpy, DefaultScreen(dpy)),
		       GCForeground | GCFont | GCGraphicsExposures, &gcvalues);
        xv_set(canvas, XV_KEY_DATA, GC_KEY, gc1, 0);
	XDrawString(dpy, xwin, gc1, 167, 40, "Cornell University", 18);
	XDrawString(dpy, xwin, gc1, 115, 65, "Center of Applied Mathematics", 29);

	font = (Xv_Font) xv_find(frame, FONT, 
				 FONT_FAMILY, FONT_FAMILY_ROMAN,
				 FONT_SIZE, 18,
				 FONT_STYLE, FONT_STYLE_BOLD, 0);
	if(!font)
	   font = (Xv_Font) xv_get(frame, XV_FONT);
	gcvalues.font = (Font) xv_get(font, XV_XID);
	gc2 = XCreateGC(dpy, RootWindow(dpy, DefaultScreen(dpy)),
		       GCForeground | GCFont | GCGraphicsExposures, &gcvalues);
        xv_set(canvas, XV_KEY_DATA, GC_KEY, gc2, 0);
	XDrawString(dpy, xwin, gc2, 90, 120, "DsTool :", 8);
	font = (Xv_Font) xv_find(frame, FONT, 
				 FONT_FAMILY, FONT_FAMILY_ROMAN,
				 FONT_SIZE, 18,
				 FONT_STYLE, FONT_STYLE_NORMAL, 0);
	if(!font)
	   font = (Xv_Font) xv_get(frame, XV_FONT);
	gcvalues.font = (Font) xv_get(font, XV_XID);
	gc2 = XCreateGC(dpy, RootWindow(dpy, DefaultScreen(dpy)),
		       GCForeground | GCFont | GCGraphicsExposures, &gcvalues);
        xv_set(canvas, XV_KEY_DATA, GC_KEY, gc2, 0);
	XDrawString(dpy, xwin, gc2, 170, 120, "A Dynamical Systems Toolkit", 27);
	XDrawString(dpy, xwin, gc2, 157, 185, "Program Installation", 21);
	font = (Xv_Font) xv_find(frame, FONT, 
				 FONT_FAMILY, FONT_FAMILY_ROMAN,
				 FONT_SIZE, 14,
				 FONT_STYLE, FONT_STYLE_BOLD, 0);
	if(!font)
	   font = (Xv_Font) xv_get(frame, XV_FONT);
	gcvalues.font = (Font) xv_get(font, XV_XID);
	gc2 = XCreateGC(dpy, RootWindow(dpy, DefaultScreen(dpy)),
		       GCForeground | GCFont | GCGraphicsExposures, &gcvalues);
        xv_set(canvas, XV_KEY_DATA, GC_KEY, gc2, 0);
	XDrawString(dpy, xwin, gc2, 375, 240, "Version 2.0", 11);

	for(i=0; i<100000; i++)
	   {
	   }
        if(first==4)
           exit(0);
        else
	   ++first ;

}
