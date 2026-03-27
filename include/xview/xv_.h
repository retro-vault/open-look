/*
 * Declares public XView interfaces, types, and attributes for xv.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(XV__H)
#define XV__H

#include <xview/pkg.h>

void xv_alloc_error(void);
void *xv_calloc(unsigned int num, unsigned int size);
Xv_private int xv_set_embedding_data(Xv_opaque object, Xv_opaque std_object);
Xv_private Xv_opaque xv_object_to_standard(Xv_opaque object, const char *caller);
Xv_public Xv_object _xv_find( Xv_opaque parent, Xv_pkg *pkg, ... );
Xv_private Xv_object xv_find_avlist(Xv_opaque parent, register Xv_pkg* pkg, Attr_attribute avlist[ATTR_STANDARD_SIZE]);
Xv_public Xv_object _xv_create(Xv_opaque parent, Xv_pkg *pkg, ...);
Xv_private Xv_object xv_create_avlist(Xv_opaque parent, register Xv_pkg *pkg, Attr_attribute avlist[ATTR_STANDARD_SIZE]);
Xv_public Xv_opaque _xv_set(Xv_opaque object, ...);
Xv_private Xv_opaque xv_set_pkg_avlist(register Xv_object object, register Xv_pkg *pkg, Attr_avlist avlist);
Xv_private Xv_opaque xv_set_avlist(Xv_opaque passed_object, Attr_avlist avlist);
Xv_public Xv_opaque xv_super_set_avlist(register Xv_opaque object, register Xv_pkg *pkg, Attr_avlist avlist);
Xv_public Xv_opaque _xv_get(Xv_opaque passed_object, Attr_attribute attr, ...);
Xv_private Xv_opaque xv_get_varargs(Xv_opaque passed_object, Attr_attribute attr, va_list valist);
Xv_private int xv_destroy_internal(Xv_opaque object, Notify_event_type check_when, Notify_event_type destroy_when, short int destruction_type);
Xv_public int xv_destroy(Xv_opaque object);
Xv_public int xv_destroy_check(Xv_opaque object);
Xv_public int xv_destroy_immediate(Xv_opaque object);
Xv_public int xv_destroy_safe(Xv_opaque object);
Xv_public int xv_destroy_save_yourself(Xv_opaque object);
Xv_public int xv_destroy_status(Xv_object passed_object, Destroy_status status);
Xv_private int xv_check_bad_attr(register Xv_pkg *pkg, Attr_attribute attr);
Xv_private int xv_has_been_initialized(void);

#endif
