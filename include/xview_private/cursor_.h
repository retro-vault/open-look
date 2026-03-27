/*
 * Declares private XView interfaces, types, and macros for cursor.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(CURSOR__H)
#define CURSOR__H

#include <xview/pkg.h>
#include <X11/Xresource.h>
#include <xview/cursor.h>
#include <xview/screen.h>

Pkg_private int cursor_create_internal(Xv_Screen parent, Xv_Cursor object, Attr_avlist avlist);
Pkg_private int cursor_destroy_internal(Xv_Cursor cursor_public, Destroy_status status);
Pkg_private Xv_opaque cursor_get_internal(Xv_Cursor cursor_public, int *status, Attr_attribute which_attr, va_list args);
Pkg_private Xv_opaque cursor_set_internal(Xv_Cursor cursor_public, register Attr_avlist avlist);
Xv_private void cursor_set_cursor(Xv_object window, Xv_Cursor cursor_public);

#endif
