/*
 * Declares internal interfaces used by the XView frame module.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#ifndef _frame_help_h_already_included
#define _frame_help_h_already_included

/* standard includes */
#ifndef FILE
#if !defined(SVR4) && !defined(__linux__)
#undef NULL
#endif /* SVR4 */
#include <stdio.h>
#endif /* FILE */
#include <sys/time.h>
#include <xview/notify.h>
#include <xview/rect.h>
#include <xview/rectlist.h>

#include <xview/win_struct.h>	/* for WL_ links */
#include <xview/win_input.h>

/* all this for wmgr.h */
#include <xview/win_screen.h>
#include <xview/wmgr.h>
#include <xview_private/wmgr_decor.h>

#include <xview/pkg.h>
#include <xview/attrol.h>
#include <xview/frame.h>
#include <xview/icon.h>
#include <xview/openmenu.h>

/* all this for XWMHints */
#include <X11/Xlib.h>
#include <X11/Xutil.h>

#define	FRAME_HELP_PRIVATE(f)	XV_PRIVATE(Frame_help_info, Xv_frame_help, f)
#define FRAME_HELP_PUBLIC(f)	XV_PUBLIC(f)
#define FRAME_CLASS_FROM_HELP(f) FRAME_PRIVATE(FRAME_HELP_PUBLIC(f))

#define FRAME_HELP_FLAGS  WMDecorationHeader

typedef	struct	{
    Frame	public_self;	/* back pointer to object */
    WM_Win_Type	win_attr;	/* _OL_WIN_ATTR */			

    struct {
	BIT_FIELD(show_label); 		/* show label or not */
    } status_bits;
} Frame_help_info;

#endif
