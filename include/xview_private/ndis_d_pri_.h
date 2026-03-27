/*
 * Declares private XView interfaces, types, and macros for ndis d pri.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(NDIS_D_PRI__H)
#define NDIS_D_PRI__H

#include <xview/pkg.h>
#include <xview_private/ntfy.h>

pkg_private Notify_value ndis_default_prioritizer(register Notify_client nclient, register int nfd, fd_set *ibits_ptr, fd_set *obits_ptr, fd_set *ebits_ptr, int nsig, register sigset_t *sigbits_ptr, register sigset_t *auto_sigbits_ptr, int *event_count_ptr, Notify_event *events, Notify_arg *args);

#endif
