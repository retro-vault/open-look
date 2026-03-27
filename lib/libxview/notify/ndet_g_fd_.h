/*
 * Declares internal interfaces used by the XView notify module.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(NDET_G_FD__H)
#define NDET_G_FD__H

#include <xview/pkg.h>
#include <xview_private/ntfy.h>

Notify_func ndet_get_fd_func(Notify_client nclient, int fd, NTFY_TYPE type);

#endif
