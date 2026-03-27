/*
 * Declares internal interfaces used by the XView attr module.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(ATTR_CUST__H)
#define ATTR_CUST__H

#include <xview/pkg.h>
#include <xview/server.h>

Xv_private Attr_avlist attr_customize(Xv_object obj, Xv_pkg *pkg, char *instance_name, Xv_object owner, Attr_attribute listhead[], int listlen, Attr_attribute avlist[]);
Xv_server attr_get_server(Xv_object obj, Xv_object passed_owner);
Xv_public int _xv_add_custom_attrs(Xv_pkg *pkg, ...);

#endif
