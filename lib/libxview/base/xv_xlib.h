/*
 * Declares internal interfaces used by the XView base module.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#ifndef _view2_private_xv_xlib_h_already_included
#define _view2_private_xv_xlib_h_already_included

#include <view2/qualifyx.h>
#include <X11/Xlib.h>
#include <view2/unqualifyx.h>

/*
 * To enable applications to get the current clipping list
 * to do direct X graphics.
 */
#define XV_MAX_XRECTS 32
typedef struct {
	XRectangle      rect_array[XV_MAX_XRECTS];
	int             count;
} Xv_xrectlist;

#endif /* _view2_private_xv_xlib_h_already_included */
