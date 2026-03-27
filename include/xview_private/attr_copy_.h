/*
 * Declares private XView interfaces, types, and macros for attr copy.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(ATTR_COPY__H)
#define ATTR_COPY__H

#include <xview/pkg.h>

int attr_copy(Attr_avlist *source, Attr_avlist *dest);

#endif
