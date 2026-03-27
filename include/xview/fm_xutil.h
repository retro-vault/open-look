/*
 * This file has been modified to include the new extensions for ICC.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#ifndef frame_xutil_h_already_included
#define frame_xutil_h_already_included
typedef struct {
	long flags;	/* marks which fields in this structure are defined */
	Bool input;	/* does this application rely on the window manager to
			get keyboard input? */
	int initial_state;	/* see below */
	Pixmap icon_pixmap;	/* pixmap to be used as icon */
	Window icon_window; 	/* window to be used as icon */
	int icon_x, icon_y; 	/* initial position of icon */
	Pixmap icon_mask;	/* icon mask bitmap */
	XID window_group;	/* id of related window group */
	/* this structure may be extended in the future */
} Frame_WMHints;

/* definition for flags of XWMHints */

#define MessageHint		(1L << 7)

/* definition for the messages flag */
#define ConfigureDenied	(1L << 0)
#define WindowMoved (1L << 1)
#define SaveYourselfMessage (1L << 2)
#define FocusMessage (1L << 3)

#endif /* frame_xutil_h_already_included */
