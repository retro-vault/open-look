/*
 * Declares public XView interfaces, types, and attributes for
 * ndetselect.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(NDETSELECT__H)
#define NDETSELECT__H

#include <xview/pkg.h>
#include <xview_private/ntfy.h>

int select(register int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout);

#endif
