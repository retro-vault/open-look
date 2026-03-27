/*
 * Declares public XView interfaces, types, and attributes for xv util.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(XV_UTIL__H)
#define XV_UTIL__H

#include <xview/pkg.h>

Xv_private int xv_get_hostname (char *buf, int maxlen);
Xv_private char *xv_strtok(char *token_string, char *sep_string);

#ifdef XV_USE_ALLOC_FUNCS
Xv_private void *xv_alloc_func(unsigned int s);
Xv_private void *xv_alloc_n_func(unsigned int s, unsigned int n);
Xv_private void *xv_malloc_func(unsigned int s);
Xv_private void *xv_realloc_func(void *p, unsigned int s);
Xv_private void *xv_valloc_func(unsigned int s);
#endif 

#ifdef XV_NO_STRDUP
char *strdup(char *s);
#endif 

#endif
