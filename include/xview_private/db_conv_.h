/*
 * Declares private XView interfaces, types, and macros for db conv.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(DB_CONV__H)
#define DB_CONV__H

#include <xview/pkg.h>

#ifdef OW_I18N
Pkg_private int db_cvt_string_to_wcs(char *from_value, Xv_opaque *to_value);
#endif 
Pkg_private int db_cvt_string_to_long(char *from_value, Xv_opaque *to_value);
Pkg_private int db_cvt_string_to_int(char *from_value, Xv_opaque *to_value);
Pkg_private int db_cvt_string_to_bool(char *from_value, Xv_opaque *to_value);
Pkg_private int db_cvt_string_to_char(char *from_value, Xv_opaque *to_value);

#endif
