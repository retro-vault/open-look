/*
 * Declares private XView interfaces, types, and macros for ndisdsched.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(NDISDSCHED__H)
#define NDISDSCHED__H

#include <xview/pkg.h>
#include <xview_private/ntfy.h>
#include <xview/notify.h>

pkg_private Notify_value ndis_default_scheduler(int n, register Notify_client *nclients);
pkg_private Notify_value ndis_special_client_scheduler(int n, register Notify_client *nclients);
void ndis_set_special_client(Notify_client client);
void ndis_unset_special_client(void);

#endif
