/*
 * Declares private XView interfaces, types, and macros for ndet fcntl.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(NDET_FCNTL__H)
#define NDET_FCNTL__H

#include <xview/pkg.h>

#ifndef __linux__
#ifdef SVR4
int xv_fcntl(int fd, int cmd, int arg);
#else
int fcntl(int fd, int cmd, int arg);
#endif /* SVR4 */
#endif

#endif
