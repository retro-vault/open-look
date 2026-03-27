/*
 * Declares private XView interfaces, types, and macros for ndet s fd.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(NDET_S_FD__H)
#define NDET_S_FD__H

#include <xview/pkg.h>
#include <xview_private/ntfy.h>

Notify_func ndet_set_fd_func(Notify_client nclient, Notify_func func, int fd, NTFY_TYPE type);

#endif
