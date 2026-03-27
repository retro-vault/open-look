/*
 * Declares public XView interfaces, types, and attributes for generic.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(GENERIC__H)
#define GENERIC__H

#include <xview/pkg.h>
#include <X11/Xresource.h>

Xv_public Attr_attribute xv_unique_key(void);
Xv_private int generic_init(Xv_object parent, Xv_object object, Attr_avlist avlist);
Xv_private XrmQuarkList generic_create_instance_qlist(Xv_object parent, char *instance_name);
Xv_private void generic_set_instance_name(Xv_object parent, Xv_object object, char* instance_name);
Xv_opaque generic_set_avlist(Xv_object object, Attr_avlist avlist);
Xv_opaque generic_get(Xv_object object, int *status, Attr_attribute attr, va_list args);
int generic_destroy(Xv_object object, Destroy_status status);

#endif
