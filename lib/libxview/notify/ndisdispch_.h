/*
 * Declares internal interfaces used by the XView notify module.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(NDISDISPCH__H)
#define NDISDISPCH__H

#include <xview/pkg.h>
#include <xview_private/ntfy.h>


pkg_private Notify_error ndis_enqueue(register NTFY_CLIENT *ndet_client, register NTFY_CONDITION *ndet_condition);
pkg_private Notify_error ndis_dispatch(void);
void notify_flush_pending(Notify_client nclient);
Notify_error notify_client(Notify_client nclient);
Notify_error notify_input(Notify_client nclient, int fd);
Notify_error notify_output(Notify_client nclient, int fd);
Notify_error notify_exception(Notify_client nclient, int fd);
Notify_error notify_itimer(Notify_client nclient, int which);
Notify_error notify_event(Notify_client nclient, Notify_event event, Notify_arg arg);
Notify_error notify_signal(Notify_client nclient, int sig);
Notify_error notify_destroy(Notify_client nclient, Destroy_status status);
Notify_error notify_wait3(Notify_client nclient);
pkg_private void ndis_flush_condition(Notify_client nclient, NTFY_TYPE type, NTFY_DATA data, int use_data);
pkg_private void ndis_flush_wait3(Notify_client nclient, int pidpid);

#endif
