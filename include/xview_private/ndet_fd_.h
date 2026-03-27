/*
 * Declares private XView interfaces, types, and macros for ndet fd.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(NDET_FD__H)
#define NDET_FD__H

#include <xview/pkg.h>
#include <xview_private/ntfy.h>


pkg_private int ndet_check_fd(int fd);

#endif
