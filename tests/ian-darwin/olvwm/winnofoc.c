/*
 *      (c) Copyright 1989 Sun Microsystems, Inc. Sun design patents
 *      pending in the U.S. and foreign countries. See LEGAL_NOTICE
 *      file for terms of the license.
 */

#ifdef IDENT
#ident	"@(#)winnofoc.c	1.4 olvwm version 07 Jan 1994"
#endif

/*
 * Based on
#ident	"@(#)winnofoc.c	26.22	91/09/14 SMI"
 *
 */

#include <errno.h>
#include <stdio.h>
#include <X11/Xos.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>

#include "i18n.h"
#include <olgx/olgx.h>

#include "ollocale.h"
#include "mem.h"
#include "olwm.h"
#include "win.h"
#include "globals.h"
#include "selection.h"

extern unsigned int FindModifierMask();
extern Atom AtomOlwmNoFocusWin;
extern Atom AtomProtocols;
extern Atom AtomTakeFocus;
extern void ClientSetCurrent();

/***************************************************************************
* global data
***************************************************************************/

Window NoFocusWin = 0;
WinNoFocus *NoFocusWinInfo;

/***************************************************************************
* private data
***************************************************************************/

static 	ClassNoFocus 	classNoFocus;

/***************************************************************************
* private functions
***************************************************************************/

/*
 * DestroyNoFocus -- destroy the no-focus window and free any allocated
 *	data.
 */
static int
/* ARGSUSED */
int destroyNoFocus(Display	* dpy, WinGeneric * winInfo)
{
	/* free our data and throw away window */
	DestroyWindow(winInfo);
	MemFree(winInfo);
}


/*
 * eventClientMessage - handle WM_TAKE_FOCUS messages.  This is a vestige of a 
 * previous focus-handling scheme.  However, it remains, in case it becomes 
 * useful at some time for the no-focus window to respond to WM_TAKE_FOCUS 
 * messages.
 */
static int
eventClientMessage(Display * dpy, XEvent * event, WinNoFocus * winInfo)
{
    if (event->xclient.message_type == AtomProtocols &&
	event->xclient.data.l[0] == AtomTakeFocus &&
	event->xclient.format == 32)
    {
	NoFocusTakeFocus(dpy, event->xclient.data.l[1],
			 winInfo->core.client->scrInfo);
    }
}


/* 
 * eventSelection
 *
 * Handle a SelectionRequest of SelectionClear event.
 */
static int
/* ARGSUSED */
int eventSelection(Display		* dpy, XEvent		* pEvent, WinNoFocus	* winInfo)
{
	SelectionResponse(pEvent);
}

/***************************************************************************
* global functions
***************************************************************************/

/*
 * MakeNoFocus  -- create the no-focus windows to a window. Return a 
 * WinGeneric structure.
 */
WinGeneric *
MakeNoFocus(Display	* dpy, WinGeneric * par)
{
	XSetWindowAttributes attributes;
	WinNoFocus *w;

	/* create the window */
	attributes.event_mask = KeyPressMask | PropertyChangeMask;
	attributes.override_redirect = True;
	NoFocusWin = XCreateWindow(dpy, par->core.self,
			 -10, -10,
			 10, 10	,
			 0,
			 0,
			 InputOnly,
			 CopyFromParent,
			 CWEventMask | CWOverrideRedirect,
			 &attributes);
	XMapWindow(dpy, NoFocusWin);

	/* create the associated structure */
	w = MemNew(WinNoFocus);
	w->core.self = NoFocusWin;
	w->core.kind = WIN_NOFOCUS;
	w->class = &classNoFocus;
	w->core.parent = par;
	w->core.children = 0;
	w->core.client = par->core.client;
	w->core.x = -10;
	w->core.y = -10;
	w->core.width = 10;
	w->core.height = 10;
	w->core.dirtyconfig = False;
	w->core.exposures = 0;
	w->core.helpstring = (char *)0; 	/* no help */

	/* register the window */
	WIInstallInfo(w);

	/* set the focus to the NoFocusWin */
	NoFocusWinInfo = w;

	/*
	 * Call TimeFresh to leave a property on the window so that it's
	 * identifiable with `xprop'.
	 */
	(void) TimeFresh();

	return w;
}

/*
 * Handle a key press in the no focus window.  If its a standard Virtual
 * Desktop function -- up, down, etc. -- then execute it, otherwise beep
 */
static int
NoFocusKey(Display	* dpy, XEvent	* event, WinGeneric * winInfo)
{
    if (!GRV.ArrowInRoot || !KeyMoveVDM(dpy, event))
	NoFocusEventBeep(dpy, event, winInfo);
}

void
/* ARGSUSED */
int NoFocusInit(Display * dpy)
{
	classNoFocus.core.kind = WIN_NOFOCUS;
	classNoFocus.core.xevents[ButtonPress] = NoFocusEventBeep;
	classNoFocus.core.xevents[ButtonRelease] = NoFocusEventBeep;
	classNoFocus.core.xevents[KeyPress] = NoFocusKey;  /*NoFocusEventBeep;
	classNoFocus.core.xevents[KeyRelease] = NoFocusEventBeep; */
	classNoFocus.core.xevents[ClientMessage] = eventClientMessage;
	classNoFocus.core.xevents[SelectionRequest] = eventSelection;
	classNoFocus.core.xevents[SelectionClear] = eventSelection;
	classNoFocus.core.focusfunc = 0;
	classNoFocus.core.drawfunc = 0;
	classNoFocus.core.destroyfunc = destroyNoFocus;
	classNoFocus.core.selectfunc = 0;
	classNoFocus.core.newconfigfunc = 0;
	classNoFocus.core.newposfunc = 0;
	classNoFocus.core.setconfigfunc = 0;
	classNoFocus.core.createcallback = 0;
	classNoFocus.core.heightfunc = 0;
	classNoFocus.core.widthfunc = 0;
}


/*
 * Set the focus to the NoFocus window.  If colormap installation is tracking 
 * the focus, install and lock the default colormap for this screen.
 */
void
NoFocusTakeFocus(Display * dpy, Time evtime, ScreenInfo * scrInfo)
{
	XSetInputFocus(dpy, NoFocusWin, RevertToParent, evtime);
	ClientSetCurrent(NoFocusWinInfo->core.client);
	ClientDefaultWindowState(dpy);
	if (scrInfo != 0 && GRV.ColorTracksInputFocus)
	    InstallDefaultColormap(dpy, scrInfo->rootwin, True);
}

/*
 * NoFocusEventBeep -- beep on keyboard/mouse events for the no-focus window
 *	Also used by busy windows
 */
int
NoFocusEventBeep(Display	* dpy, XEvent	* event, WinGeneric * winInfo)
{
        XEvent dummy;

	if (FindModifierMask(event->xkey.keycode) != 0)
		return 0;

        XSync(dpy,0);
        /*
         * Discard pending keyboard and mouse events on this
         * window, and then beep once.
         */
        while (XCheckTypedWindowEvent(dpy,winInfo->core.self,
                                      KeyPress,&dummy))
	    /*EMPTY*/
            ;
        while (XCheckTypedWindowEvent(dpy,winInfo->core.self,
                                      KeyRelease,&dummy))
	    /*EMPTY*/
            ;
        while (XCheckTypedWindowEvent(dpy,winInfo->core.self,
                                      ButtonPress,&dummy))
	    /*EMPTY*/
            ;
        while (XCheckTypedWindowEvent(dpy,winInfo->core.self,
                                      ButtonRelease,&dummy))
	    /*EMPTY*/
            ;
	KeyBeep(dpy,event);
}

/*
 * TimeFresh
 *
 * Get a fresh timestamp from the server.  This is accomplished by writing a
 * zero-length property on the no-focus window and getting the timestamp out
 * of the resulting PropertyNotify event.
 */
Time
TimeFresh(void)
{
	XEvent e;
	Time timestamp;

	XChangeProperty(NoFocusWinInfo->core.client->dpy, NoFocusWin,
		AtomOlwmNoFocusWin, XA_INTEGER,
		32, PropModeReplace, (unsigned char *)NoFocusWinInfo, 0);
	XSync(NoFocusWinInfo->core.client->dpy, False);
	if (XCheckTypedWindowEvent(NoFocusWinInfo->core.client->dpy,
				   NoFocusWin, PropertyNotify, &e))
	    timestamp = e.xproperty.time;
	else
	    timestamp = CurrentTime;

	return timestamp;
}
