/*
 *      (c) Copyright 1989 Sun Microsystems, Inc. Sun design patents
 *      pending in the U.S. and foreign countries. See LEGAL_NOTICE
 *      file for terms of the license.
 */

#ifdef IDENT
#ident	"@(#)win.c	1.4 olvwm version 07 Jan 1994"
#endif

/*
 * Based on
#ident	"@(#)win.c	26.16	93/06/28 SMI"
 *
 */

#include <stdio.h>
#include <memory.h>
#include <X11/Xos.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>

#include "i18n.h"
#include "olwm.h"
#include "win.h"
#include "mem.h"
#include "st.h"
#include "notice.h"

/***************************************************************************
* local functions
***************************************************************************/

/*ARGSUSED*/
static enum st_retval
doRedrawOneWindow(Window w, WinGeneric * win, void * c)
{
    if ((WinFunc(win,core.drawfunc) != 0) && (win->core.client != 0))
	(WinFunc(win,core.drawfunc))(win->core.client->dpy,win);
    return ST_CONTINUE;
}

static void
callSelectTree(Display * dpy, WinGeneric * win, Bool sel)
{
	List *l = win->core.children;

	if (WinFunc(win,core.selectfunc) != 0) {
		(*(WinFunc(win,core.selectfunc)))(dpy,win,sel);
		VirtualSelect(win, sel);
	}
	for (win = ListEnum(&l); win != 0; win = ListEnum(&l))
		callSelectTree(dpy, win, sel);
}

/* Destroying a window tree:  walk the trees associated with a client, 
 * invoking the destroyfuncs on each.  The order that the windows are
 * destroyed is significant; it works from the root upwards.  The
 * client structure is not affected; it is assumed that this function
 * is only called from routines which take responsibility for removing
 * the client structure as well.
 */
static void
callDestroyTree(Display * dpy, WinGeneric * win)
{
	List *l;
	WinGeneric *cwin;

	if (win == 0)
		return;
	l = win->core.children;
	for (cwin = ListEnum(&l); cwin != 0; cwin = ListEnum(&l))
		callDestroyTree(dpy, cwin);
	if (WinFunc(win,core.destroyfunc) != 0)
		(*(WinFunc(win,core.destroyfunc)))(dpy,win);
}

static void
setTreeConfig(Display * dpy, WinGeneric * win)
{
	List *l = win->core.children;
	WinGeneric *wc;

	(*(WinFunc(win,core.setconfigfunc)))(dpy,win);
	for (wc = ListEnum(&l); wc != 0; wc = ListEnum(&l))
	{
		setTreeConfig(dpy,wc);
	}
}

static void
callFocusTree(WinGeneric * win, Bool focus)
{
	List *l;
	WinGeneric *cwin;

	if (win == 0)
		return;
	if (WinFunc(win,core.focusfunc) != 0) {
		(WinFunc(win,core.focusfunc))(win->core.client->dpy,win,focus);
	}
	l = win->core.children;
	for (cwin = ListEnum(&l); cwin != 0; cwin = ListEnum(&l))
		callFocusTree(cwin, focus);
}


static void
callDrawTree(WinGeneric * win)
{
    List *l;
    WinGeneric *cwin;

    if (WinFunc(win,core.drawfunc) != 0)
	(WinFunc(win,core.drawfunc))(win->core.client->dpy,win);

    l = win->core.children;
    for (cwin = ListEnum(&l); cwin != 0; cwin = ListEnum(&l))
	callDrawTree(cwin);
}


/***************************************************************************
* global functions
***************************************************************************/

/* WinCallSelect - call a client's select functions for either the icon or
 *   frame trees, depending on which is visible.  Passes along sel,
 *   which is True iff the client is being selected.
 */
void
WinCallSelect(Client * cli, Bool sel)
{
	if (cli->wmState == NormalState)
		callSelectTree(cli->dpy, cli->framewin, sel);
	else if (cli->wmState == IconicState)
		callSelectTree(cli->dpy, cli->iconwin, sel);
}

/* WinCallFocus - call a client's focus functions for the frame tree.
 *   Passes along focus, which is True iff the client is gaining focus
 */
void
WinCallFocus(WinGeneric * win, Bool focus)
{
	if (win == 0)
		return;
	win->core.client->isFocus = focus;
	callFocusTree(win, focus);
	VirtualChangeFocus(win, focus);
}
	
/* WinCallDestroy - call a client's destroy functions for both the icon and
 *   frame trees
 */
void
WinCallDestroy(Client * cli)
{
	Display *dpy = cli->dpy;
	WinPaneFrame *framewin = cli->framewin;
	WinIconFrame *iconwin = cli->iconwin;

	callDestroyTree(dpy, framewin);
	callDestroyTree(dpy, iconwin);
}

/* WinCallConfig - initiate a configuration change, starting at some
 *	particular window.
 *	Configuration change works as follows:  a window is the initiator
 *	of the change.  If the children of this window need to be sized,
 *	then they are called to do so; then the window sizes itself and
 *	sets the position of each of its children.  This process is
 *	repeated on the parent of the initiating window, and so on up
 *	to the top window in the hierarchy (presumably a frame).
 *	A second pass then occurs, doing a depth-first preorder 
 *	traversal of the window tree, performing the window's set
 *	configuration function to make the computed change.
 *	In the first pass, any window which changes its configuration 
 *	should return this fact to be propagated back to this routine;
 *	if no window has changed configuration then the second pass
 *	won't be performed.
 *	The initiator window is passed (in some cases) the configure
 *	request event which cause the action to begin.
 */
void
WinCallConfig(Display * dpy, WinGeneric * win, XConfigureRequestEvent * pxcre)
{
	Bool fDirty = False;
	WinGeneric *w;
	
	do
	{
		fDirty = (WinFunc(win,core.newconfigfunc))(win,pxcre) || fDirty;
		pxcre = 0;
		w = win;
	}
	while ((win = win->core.parent) != 0);
	if (fDirty)
	{
		setTreeConfig(dpy,w);
	}
}

/*
 * WinCallDraw
 * Call all child windows' draw functions.
 */
void
WinCallDraw(WinGeneric * win)
{
    callDrawTree(win);
}


/* WinAddChild -- add a child to a parent's list of children
 */
void
WinAddChild(WinGeneric * parent, WinGeneric * child)
{
	parent->core.children = ListCons(child,parent->core.children);
	child->core.parent = parent;
}

/* WinRemoveChild -- remove a child from a parent's list of children
 */
void
WinRemoveChild(WinGeneric * parent, WinGeneric * child)
{
	List **l;

	for (l = &(parent->core.children); *l != 0; l = &((*l)->next))
	{
	    if ((WinGeneric *)((*l)->value) == child)
	    {
		ListDestroyCell(l);
		return;
	    }
	}
#ifdef DEBUG
	printf("Warning:  tried to remove child %x from parent %x, but it wasn't there\n",child,parent);
#endif
}

/* WinRootPos -- figure the root coordinates of a window's position
 */
void
WinRootPos(WinGeneric * win, int * px, int *py)
{
	*px = 0;
	*py = 0;
	for ( ; win != 0; win = win->core.parent)
	{
		*px += win->core.x;
		*py += win->core.y;
	}
}


/* WinRedrawAllWindows -- call every window's draw function (if provided)
 */
void
WinRedrawAllWindows(void)
{
	WIApply(doRedrawOneWindow, 0);
}

/* WinShowHelp -- show help for that window if available
 */
Bool
WinShowHelp(Display		* dpy, WinGeneric	* win, int mousex, int mousey)
{
	int	screen;
	Text	*buttons[1];
	Text	*msg;
	NoticeBox noticeBox;

	if (win->core.helpstring) {
		if (win->core.client)
			screen = win->core.client->screen;
		else
			screen = DefaultScreen(dpy);

		if (!ShowHelpWindow(screen,mousex,mousey,
		        win->core.helpstring)) {

			buttons[0] = GetText("Ok");
			msg = 
	GetText("Couldn't write to olwmslave\nNo Help Available for olwm");

			noticeBox.numButtons = NOTICE_BUTTON_COUNT(buttons);
			noticeBox.defaultButton = 0;
			noticeBox.buttonText = buttons;
			noticeBox.msgText = msg;
			noticeBox.boxX = -1;	/* centered */
			noticeBox.boxY = -1;
			UseNoticeBox(dpy,screen,&noticeBox);
			FreeText(buttons[0]);
			FreeText(msg);
			return False;
		}
		return True;
	}
	return False;
}

/***************************************************************************
* general event/class functions
***************************************************************************/

int
WinDrawFunc(WinGeneric * win)
{
	if ((WinFunc(win,core.drawfunc) != 0) && (win->core.client != 0))
		(WinFunc(win,core.drawfunc))(win->core.client->dpy,win);
}

int
WinEventExpose(Display * dpy, XEvent * event, WinGeneric * win)
{
	if (event->xexpose.count == 0)
	    (WinFunc(win,core.drawfunc))(dpy, win);
}


int
WinNewPosFunc(WinGeneric * win, int x, int y)
{
    if (x != win->core.x)
    {
	win->core.x = x;
	win->core.dirtyconfig |= CWX;
    }

    if (y != win->core.y)
    {
	win->core.y = y;
	win->core.dirtyconfig |= CWY;
    }

    return win->core.dirtyconfig;
}


int
WinNewConfigFunc(WinGeneric * win, XConfigureRequestEvent * pxcre)
{
	int neww = WinFunc(win,core.widthfunc)(win, pxcre);
	int newh = WinFunc(win,core.heightfunc)(win, pxcre);

	if (neww != win->core.width)
	{
	    win->core.width = neww;
	    win->core.dirtyconfig |= CWWidth;
	}
	if (newh != win->core.height)
	{
	    win->core.height = newh;
	    win->core.dirtyconfig |= CWHeight;
	}
	return win->core.dirtyconfig;
}


int
WinSetConfigFunc(Display * dpy, WinGeneric * win)
{
	XWindowChanges xwc;
	int	dw = DisplayWidth(win->core.client->dpy,
				  win->core.client->screen);
	int	dh = DisplayHeight(win->core.client->dpy,
				   win->core.client->screen);

	if (win->core.dirtyconfig)
	{
	    if (win->core.client->sticky) {
		if (win->core.x + (int) win->core.width < 0) {
		    win->core.x = dw + (win->core.x % dw);
		    win->core.dirtyconfig |= CWX;
		}
		else if (win->core.x >= dw) {
		    win->core.x = win->core.x % dw;
		    win->core.dirtyconfig |= CWX;
		}
		if (win->core.y + (int) win->core.height < 0) {
		    win->core.y = dh + (win->core.y % dh);
		    win->core.dirtyconfig |= CWY;
		}
		else if (win->core.y >= dh) {
		    win->core.y = win->core.y % dh;
		    win->core.dirtyconfig |= CWY;
		}
	    }
	    xwc.x = win->core.x;
	    xwc.y = win->core.y;
	    xwc.width = win->core.width;
	    xwc.height = win->core.height;
	    /* generic windows never change border or stacking */
	    ConfigureWindow(dpy,win,win->core.dirtyconfig,&xwc);
	}
	win->core.dirtyconfig &= ~(CWX|CWY|CWWidth|CWHeight);
}


/*
 * WinAddColorClient
 *
 * Add cli to this win's list of colormap clients.  Assumes that cli isn't 
 * already in the list.
 */
void
WinAddColorClient(WinGeneric * win, Client * cli)
{
    win->core.colormapClients = ListCons(cli, win->core.colormapClients);
}


/*
 * WinRemoveColorClient
 *
 * Remove cli from this win's list of colormap clients.  If there are no more
 * clients, and this window is a WIN_COLORMAP, destroy the window.  Assumes
 * that cli appears in win's list zero or one times.
 */
void
WinRemoveColorClient(Display * dpy, WinGeneric * win, Client * cli)
{
    List **l;

    l = &win->core.colormapClients;
    while (*l != 0) {
	if ((*l)->value == cli) {
	    ListDestroyCell(l);
	    break;
	}
	l = &((*l)->next);
    }
    if (win->core.colormapClients == NULL_LIST
	&& win->core.kind == WIN_COLORMAP)
	(WinFunc(win, core.destroyfunc))(dpy, win);
}
