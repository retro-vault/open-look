/*
 * Declares public XView interfaces, types, and attributes for dnd.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(DND__H)
#define DND__H

#include <xview/pkg.h>
#include <X11/Xresource.h>
#include <xview_private/dndimpl.h>

Xv_public int dnd_send_drop(Xv_object dnd_public);
Xv_private int DndSendPreviewEvent(Dnd_info *dnd, int site, XButtonEvent *e);
Pkg_private int DndWaitForEvent(Display *dpy, Window window, int eventType, Atom target, struct timeval *timeout, XEvent *event, int (*MatchFunc)());
Pkg_private int DndMatchEvent(Display *dpy, XEvent *event, DnDWaitEvent *wE);

#endif
