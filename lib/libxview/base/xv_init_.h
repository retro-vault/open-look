/*
 * Declares internal interfaces used by the XView base module.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(XV_INIT__H)
#define XV_INIT__H

#include <xview/pkg.h>
#include <X11/Xlib.h>

Xv_public Xv_object _xv_init(Attr_attribute attr1, ...);
Xv_private int xv_handle_xio_errors(Display *display);
Xv_private void xv_connection_error(char *server_name);
Xv_private char *xv_base_name(char* fullname);

#endif
