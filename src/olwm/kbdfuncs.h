#ident	"@(#)kbdfuncs.h	1.8	93/06/28 SMI"

/*
 *      (c) Copyright 1989 Sun Microsystems, Inc.
 */

/*
 *      Sun design patents pending in the U.S. and foreign countries. See
 *      LEGAL_NOTICE file for terms of the license.
 */

#ifndef _OLWM_KBDFUNCS_H
#define _OLWM_KBDFUNCS_H

#ifdef __STDC__
extern void KeyBeep(Display *dpy, XKeyEvent *ke);
extern void KeyFocusToPointer(Display *dpy, XKeyEvent *ke);
extern void KeyFrontFocus(Display *dpy, XKeyEvent *ke);
extern void KeyLockColormap(Display *dpy, XKeyEvent *ke);
extern void KeyUnlockColormap(Display *dpy, XKeyEvent *ke);
#else
extern void KeyBeep();
extern void KeyFocusToPointer();
extern void KeyFrontFocus();
extern void KeyLockColormap();
extern void KeyUnlockColormap();
#endif

extern void KeyBackFocus();
extern void KeyRaiseLowerPointer();
extern void KeyFullRestore();
extern void KeyMove();
extern void KeyNextApp();
extern void KeyNextWindow();
extern void KeyOpenClosePointer();
extern void KeyOpenCloseFocus();
extern void KeyOwner();
extern void KeyPrevApp();
extern void KeyPrevWindow();
extern void KeyProperties();
extern void KeyQuit();
extern void KeyRefresh();
extern void KeyResize();
extern void KeyToggleInput();
extern void KeyTogglePin();
extern void KeyWindowMenu();
extern void KeyWorkspaceMenu();
extern void KeyMakeInvisiblePointer();
extern void KeyMakeInvisibleFocus();
extern void KeyMakeVisibleAll();

#endif /* _OLWM_KBDFUNCS_H */
