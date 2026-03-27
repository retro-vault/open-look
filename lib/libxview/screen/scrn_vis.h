/*
 * Declares internal interfaces used by the XView screen module.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#ifndef _xview_screen_visual_h_already_included
#define _xview_screen_visual_h_already_included

#include <xview/base.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

typedef struct screen_visual {
    Xv_object		screen;
    Xv_object		server;
    Display	       *display;
    Xv_object		root_window;
    XVisualInfo        *vinfo;
    unsigned int	depth;
    Xv_opaque		colormaps;	/* List of colormaps to use with this visual (first is the default) */
    GC			gc;
    XImage	       *image_bitmap;
    XImage	       *image_pixmap;
    struct screen_visual *next;
} Screen_visual;

#endif
