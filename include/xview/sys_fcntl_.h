/*
 * Declares public XView interfaces, types, and attributes for sys
 * fcntl.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(SYS_FCNTL__H)
#define SYS_FCNTL__H

#include <xview/pkg.h>
#include <xview_private/ntfy.h>

pkg_private int notify_fcntl(int fd, int cmd, int arg);

#endif
