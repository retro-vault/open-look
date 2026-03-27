/*
 * Declares internal interfaces used by the XView win module.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(WIN_CURSOR__H)
#define WIN_CURSOR__H

#include <xview/pkg.h>
#include <X11/Xresource.h>

void win_setmouseposition(Xv_object window, short x, short y);
void win_setmouseposition_internal(Display *display, XID xid, short x, short y);
void win_getmouseposition(Xv_object window, short *x, short *y);
XID win_findintersect(Xv_object window, short x, short y);

#endif
