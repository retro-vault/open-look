/*
 * Declares public XView interfaces, types, and attributes for xv
 * casecmp.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(XV_CASECMP__H)
#define XV_CASECMP__H

#include <xview/pkg.h>

int xv_strcasecmp(char *str1, char *str2);
int xv_strncasecmp(char *str1, char *str2, int n);
#ifdef OW_I18N
int xv_wscasecmp(CHAR *str1, CHAR *str2);
int xv_wsncasecmp(CHAR *str1, CHAR *str2, int n);
#endif 

#endif
