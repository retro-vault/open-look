/*
 * Implements the xv icccm routines used by the XView misc module.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#include <xview_private/xv_icccm_.h>
#include <X11/Xatom.h>
#include <X11/Xutil.h>

#ifdef PRE_R4_ICCCM

Status
xv_iconify_window(dpy, win, scrn)
	Display *dpy;
	Window win;
	int scrn;
{
	XClientMessageEvent evt;
	Atom wm_chg_state;

	if ((wm_chg_state = XInternAtom(dpy, "WM_CHANGE_STATE", False))==None) {
		return(False);
	}

	evt.type = ClientMessage;
	evt.window = win;
	evt.message_type = wm_chg_state;
	evt.format = 32;
	evt.data.l[0] = IconicState;
	return (XSendEvent (dpy, RootWindow(dpy, scrn), False,
		SubstructureRedirectMask | SubstructureNotifyMask,
		(XEvent *)&evt));
}

Status
xv_withdraw_window(dpy, win, scrn)
	Display *dpy;
	Window win;
	int scrn;
{
	XUnmapEvent evt;

	XUnmapWindow (dpy, win);

	evt.type = UnmapNotify;
	evt.window = win;
	evt.event = RootWindow(dpy, scrn);
	evt.from_configure = False;
	return(XSendEvent (dpy, evt.event, False, 
		SubstructureRedirectMask | SubstructureNotifyMask,
		(XEvent *)&evt));
}

#endif /* PRE_R4_ICCCM */
