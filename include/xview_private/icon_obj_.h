/*
 * Declares private XView interfaces, types, and macros for icon obj.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(ICON_OBJ__H)
#define ICON_OBJ__H

#include <xview/pkg.h>
#include <xview/icon.h>

Icon _icon_create(Attr_attribute attr1, ...);
int icon_init(Xv_opaque parent, Xv_opaque object, Xv_opaque *avlist);
void icon_destroy(Icon icon_public);
int icon_destroy_internal(Icon icon_public, Destroy_status status);
int _icon_set(Icon icon_public, ...);
Xv_opaque icon_set_internal(Icon icon_public, register Attr_avlist avlist);
Xv_opaque icon_get(register Icon icon_public, Icon_attribute attr);
Xv_opaque icon_get_internal(Icon icon_public, int *status, Attr_attribute attr, va_list args);

#endif
