/*
 * Declares public XView interfaces, types, and attributes for ndet
 * wait.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(NDET_WAIT__H)
#define NDET_WAIT__H

#include <xview/pkg.h>
#include <xview_private/ntfy.h>

pkg_private int ndet_check_pid(int pid);

#endif
