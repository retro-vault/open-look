/*
 * Declares public XView interfaces, types, and attributes for curs
 * cmpat.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(CURS_CMPAT__H)
#define CURS_CMPAT__H

#include <xview/pkg.h>
#include <X11/Xresource.h>
#include <xview/cursor.h>

Xv_Cursor _cursor_create(Attr_attribute attr1, ...);
void cursor_destroy(Xv_Cursor cursor_public);
Xv_opaque cursor_get(Xv_Cursor cursor_public, Cursor_attribute which_attr);
int _cursor_set(Xv_Cursor cursor_public, ...);
Xv_Cursor cursor_copy(register Xv_Cursor cursor_publi);

#endif
