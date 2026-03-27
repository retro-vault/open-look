/*
 * Declares public XView interfaces, types, and attributes for ndet
 * read.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(NDET_READ__H)
#define NDET_READ__H

#include <xview/pkg.h>
#include <xview_private/ntfy.h>


int read(register int fd, char *buf, int nbytes);

#endif
