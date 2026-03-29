/*
 * Declares the vroot interfaces used by the xtoolplaces application.
 *
 * Copyright (c) 1991 Andreas Stolcke.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
/*
 * vroot.h -- Virtual Root Window handling header file
 *
 * This header file redefines the X11 macros RootWindow and DefaultRootWindow,
 * making them look for a virtual root window as provided by certain `virtual'
 * window managers like swm and tvtwm. If none is found, the ordinary root
 * window is returned, thus retaining backward compatibility with standard
 * window managers.
 * The function implementing the virtual root lookup remembers the result of
 * its last invocation to avoid overhead in the case of repeated calls
 * on the same display and screen arguments. 
 * The lookup code itself is taken from Tom LaStrange's ssetroot program.
 *
 * Most simple root window changing X programs can be converted to using
 * virtual roots by just including
 *
 * #include <X11/vroot.h>
 *
 * after all the X11 header files.  It has been tested on such popular
 * X clients as xphoon, xfroot, xloadimage, and xaqua.
 * It also works with the core clients xprop, xwininfo, xwd, and editres
 * (and is necessary to get those clients working under tvtwm).
 * It does NOT work with xsetroot; get the xsetroot replacement included in
 * the tvtwm distribution instead.
 *
 * Andreas Stolcke <stolcke@ICSI.Berkeley.EDU>, 9/7/90
 * - replaced all NULL's with properly cast 0's, 5/6/91
 * - free children list (suggested by Mark Martin <mmm@cetia.fr>), 5/16/91
 * - include X11/Xlib.h and support RootWindowOfScreen, too 9/17/91
 *
 * Adam Stein <adam@iset.scan.mc.xerox.com>, 9/5/94
 * - For xtoolplaces, added __CENTERLINE__ to list of possibly defined
 *   items to see if vroot_rcsid[] should be declard (__CENTERLINE__ is the
 *   updated term for SABER).
 */

#ifndef _VROOT_H_
#define _VROOT_H_

#if !defined(lint) && !defined(SABER) && !defined(__CENTERLINE__)
static char vroot_rcsid[] = "$Id: vroot.h,v 1.4 1991/09/30 19:23:16 stolcke Exp stolcke $";
#endif

#include <X11/X.h>
#include <X11/Xatom.h>
#include <X11/Xlib.h>

static Window
VirtualRootWindowOfScreen(screen)
	Screen *screen;
{
	static Screen *save_screen = (Screen *)0;
	static Window root = (Window)0;

	if (screen != save_screen) {
		Display *dpy = DisplayOfScreen(screen);
		Atom __SWM_VROOT = None;
		int i;
		Window rootReturn, parentReturn, *children;
		unsigned int numChildren;

		root = RootWindowOfScreen(screen);

		/* go look for a virtual root */
		__SWM_VROOT = XInternAtom(dpy, "__SWM_VROOT", False);
		if (XQueryTree(dpy, root, &rootReturn, &parentReturn,
				 &children, &numChildren)) {
			for (i = 0; i < numChildren; i++) {
				Atom actual_type;
				int actual_format;
				unsigned long nitems, bytesafter;
				Window *newRoot = (Window *)0;

				if (XGetWindowProperty(dpy, children[i],
					__SWM_VROOT, 0, 1, False, XA_WINDOW,
					&actual_type, &actual_format,
					&nitems, &bytesafter,
					(unsigned char **) &newRoot) == Success
				    && newRoot) {
				    root = *newRoot;
				    break;
				}
			}
			if (children)
				XFree((char *)children);
		}

		save_screen = screen;
	}

	return root;
}

#undef RootWindowOfScreen
#define RootWindowOfScreen(s) VirtualRootWindowOfScreen(s)

#undef RootWindow
#define RootWindow(dpy,screen) VirtualRootWindowOfScreen(ScreenOfDisplay(dpy,screen))

#undef DefaultRootWindow
#define DefaultRootWindow(dpy) VirtualRootWindowOfScreen(DefaultScreenOfDisplay(dpy))

#endif /* _VROOT_H_ */
