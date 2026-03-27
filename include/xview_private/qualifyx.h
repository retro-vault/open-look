/*
 * Declares private XView interfaces, types, and macros for qualifyx.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#ifndef _view2_private_qualify_x_h_already_included
#define _view2_private_qualify_x_h_already_included

/*
 * The following definitions rename those Xlib types
 * that are in conflict with Sv (or likely to be).
 * Atom could not be redefined as Xatom.h use "Atom" in
 * the defining macros. The preprocessor does not replace strings
 * in macro definitions.
 */
#define Colormap	XColormap_t
#define Cursor		XCursor_t
#define Depth		XDepth_t
#define Device		XDevice_t
#define Display		XDisplay_t
#define Drawable	XDrawable_t
#define Font		XFont_t
#define Pixmap		XPixmap_t
#define Screen		XScreen_t
#define ScreenFormat	XScreenFormat_t
#define Visual		XVisual_t
#define Window		XWindow_t

#endif /* _view2_private_qualify_x_h_already_included */
