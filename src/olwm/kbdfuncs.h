/*
 * kbdfuncs.h: declarations and shared types for the kbdfuncs module.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */

#ifndef _OLWM_KBDFUNCS_H
#define _OLWM_KBDFUNCS_H

/* KeyBeep: Beep key. */
extern void KeyBeep(Display *dpy, XKeyEvent *ke);
/* KeyFocusToPointer: Focus key to pointer. */
extern void KeyFocusToPointer(Display *dpy, XKeyEvent *ke);
/* KeyFrontFocus: Focus key front. */
extern void KeyFrontFocus(Display *dpy, XKeyEvent *ke);
/* KeyLockColormap: Lock key colormap. */
extern void KeyLockColormap(Display *dpy, XKeyEvent *ke);
/* KeyUnlockColormap: Unlock key colormap. */
extern void KeyUnlockColormap(Display *dpy, XKeyEvent *ke);

/* KeyBackFocus: Focus key back. */
extern void KeyBackFocus();
/* KeyRaiseLowerPointer: Raise key lower pointer. */
extern void KeyRaiseLowerPointer();
/* KeyFullRestore: Full key restore. */
extern void KeyFullRestore();
/* KeyMove: Move key. */
extern void KeyMove();
/* KeyNextApp: Select the next key app. */
extern void KeyNextApp();
/* KeyNextWindow: Select the next key window. */
extern void KeyNextWindow();
/* KeyOpenClosePointer: Open key close pointer. */
extern void KeyOpenClosePointer();
/* KeyOpenCloseFocus: Open key close focus. */
extern void KeyOpenCloseFocus();
/* KeyOwner: Owner key. */
extern void KeyOwner();
/* KeyPrevApp: Select the prev key app. */
extern void KeyPrevApp();
/* KeyPrevWindow: Select the prev key window. */
extern void KeyPrevWindow();
/* KeyProperties: Process key properties. */
extern void KeyProperties();
/* KeyQuit: Quit key. */
extern void KeyQuit();
/* KeyRefresh: Refresh key. */
extern void KeyRefresh();
/* KeyResize: Resize key. */
extern void KeyResize();
/* KeyToggleInput: Toggle key input. */
extern void KeyToggleInput();
/* KeyTogglePin: Toggle key pin. */
extern void KeyTogglePin();
/* KeyWindowMenu: Process key window menu. */
extern void KeyWindowMenu();
/* KeyWorkspaceMenu: Process key workspace menu. */
extern void KeyWorkspaceMenu();
/* KeyMakeInvisiblePointer: Create key invisible pointer. */
extern void KeyMakeInvisiblePointer();
/* KeyMakeInvisibleFocus: Create key invisible focus. */
extern void KeyMakeInvisibleFocus();
/* KeyMakeVisibleAll: Create key visible all. */
extern void KeyMakeVisibleAll();

#endif /* _OLWM_KBDFUNCS_H */
