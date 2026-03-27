/*
 * Declares public XView interfaces, types, and attributes for
 * expandname.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(EXPANDNAME__H)
#define EXPANDNAME__H

#include <xview/pkg.h>
#include <xview/expandname.h>


struct namelist *xv_expand_name(char name[]);
struct namelist *xv_mk_0list(void);
struct namelist *xv_mk_1list(char *str);
struct namelist *makelist(register int len, register char *str);
void free_namelist(struct namelist *ptr);

#endif
