/*
 * Declares private XView interfaces, types, and macros for sys read.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(SYS_READ__H)
#define SYS_READ__H

#include <xview/pkg.h>
#include <xview_private/ntfy.h>


pkg_private int notify_read(int fd, char *buf, int nbytes);

#endif
