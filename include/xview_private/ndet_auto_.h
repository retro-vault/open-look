/*
 * Declares private XView interfaces, types, and macros for ndet auto.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(NDET_AUTO__H)
#define NDET_AUTO__H

#include <xview/pkg.h>
#include <xview_private/ntfy.h>

pkg_private void ndet_toggle_auto(sigset_t *old_bits, register int sig);
pkg_private Notify_value ndet_auto_sig_func(Notify_client nclient, int sig, Notify_signal_mode mode);
pkg_private NTFY_ENUM ndet_auto_sig_send(NTFY_CLIENT *client, register NTFY_CONDITION *condition, NTFY_ENUM_DATA context);
pkg_private NTFY_ENUM ndet_auto_sigchld(NTFY_CLIENT *client, register NTFY_CONDITION *condition, NTFY_ENUM_DATA context);

#endif
