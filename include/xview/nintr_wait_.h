/*
 * Declares public XView interfaces, types, and attributes for nintr
 * wait.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(NINTR_WAIT__H)
#define NINTR_WAIT__H

#include <xview/pkg.h>
#include <xview_private/ntfy.h>

Notify_error notify_remove_wait3_func(Notify_client nclient, Notify_func func, int pid);

#endif
