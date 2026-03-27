/*
 * Declares internal interfaces used by the XView attr module.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(ATTR_UTIL__H)
#define ATTR_UTIL__H

#include <xview_private/attr_.h>

Attr_avlist _attr_create_list(Attr_attribute attr1, ...);
Attr_avlist attr_find(register Attr_avlist attrs, register Attr_attribute attr);

#endif
