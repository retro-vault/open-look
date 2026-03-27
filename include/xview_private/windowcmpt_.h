/*
 * Declares private XView interfaces, types, and macros for windowcmpt.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(WINDOWCOMPT__H)
#define WINDOWCOMPT__H

#include <xview_private/attr_cu_.h>
#include <xview/pkg.h>
#include <xview_private/windowimpl.h>

Xv_Window _window_create(Xv_Window parent_public, Xv_pkg *pkg, ...);
int _window_set(Xv_Window win_public, ...);
Xv_private void window_rc_units_to_pixels(Xv_Window win_public, Attr_avlist avlist);
Xv_private void window_scan_and_convert_to_pixels(Xv_Window win_public, Attr_avlist avlist);
int window_destroy(Xv_Window window);

#endif
