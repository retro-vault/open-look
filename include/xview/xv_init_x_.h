/*
 * Declares public XView interfaces, types, and attributes for xv init
 * x.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(XV_INIT_X__H)
#define XV_INIT_X__H

#include <X11/Xlib.h>
#include <xview/pkg.h>

Xv_private void xv_set_default_font(register Display *display, int screen, Font font);
#ifdef _XV_DEBUG
Xv_private void xv_dump_fonts(Display* display);
#endif
Xv_private void xv_x_error_handler(Display *dpy, XErrorEvent *event);

#endif
