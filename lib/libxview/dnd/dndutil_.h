/*
 * Declares internal interfaces used by the XView dnd module.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(DNDUTIL__H)
#define DNDUTIL__H

#include <xview/pkg.h>
#include <X11/Xresource.h>
#include <xview_private/dndimpl.h>

Xv_private XID DndGetCursor(Dnd_info *dnd);
Xv_private int DndGetSelection(Dnd_info *dnd, Display *dpy);
Pkg_private int DndSendEvent(Display *dpy, XAnyEvent *event);
Pkg_private int DndMatchProp(Display *dpy, XEvent *event, DnDWaitEvent *wE);

#endif
