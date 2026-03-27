/*
 * Declares private XView interfaces, types, and macros for attr.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(ATTR__H)
#define ATTR__H

#include <xview/pkg.h>

Attr_avlist attr_make_count(Attr_avlist listhead, int listlen, va_list valist, int* countptr);
Xv_private char *attr_name(Attr_attribute attr);
Xv_private Attr_avlist copy_va_to_av(va_list valist1, Attr_avlist avlist1, Attr_attribute attr1);
Attr_avlist attr_copy_avlist(register Attr_avlist dest, register Attr_avlist avlist);
int attr_count(Attr_avlist avlist);
int attr_count_avlist(register Attr_avlist avlist, register Attr_attribute last_attr);
Attr_avlist attr_skip_value(register Attr_attribute attr, register Attr_avlist avlist);

#endif
