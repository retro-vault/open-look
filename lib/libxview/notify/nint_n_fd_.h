/*
 * Declares internal interfaces used by the XView notify module.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(NINT_N_FD__H)
#define NINT_N_FD__H

#include <xview/pkg.h>
#include <xview_private/ntfy.h>

pkg_private Notify_value nint_next_fd_func(Notify_client nclient, NTFY_TYPE type, int fd);

#endif
