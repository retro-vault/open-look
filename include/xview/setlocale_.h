/*
 * Declares public XView interfaces, types, and attributes for
 * setlocale.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(SETLOCALE__H)
#define SETLOCALE__H

#include <xview_private/i18n_impl.h>

#ifndef OS_HAS_LOCALE
char *setlocale(int category, char *locale);
#endif 

#endif
