/*
 * Declares internal interfaces used by the XView notify module.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(NDET_LOOP__H)
#define NDET_LOOP__H

#include <xview/pkg.h>
#include <xview_private/ntfy.h>

#ifdef lint
int syscall(int a);
#endif

Notify_error notify_start(void);
Notify_error notify_stop(void);
pkg_private void ndet_update_virtual_itimer(void);
pkg_private void ndet_update_real_itimer(void);
pkg_private void ndet_enable_sig(int sig);
pkg_private void ndet_signal_catcher(int sig, siginfo_t* code, void* ucontext);
pkg_private void ndet_send_delayed_sigs(void);
pkg_private NTFY_ENUM ndet_fd_send(NTFY_CLIENT *client, register NTFY_CONDITION *condition, NTFY_ENUM_DATA context);
void notify_set_signal_check(struct timeval tv);
struct timeval notify_get_signal_check(void);
int notify_get_signal_code(void);
#if !defined(SVR4) && !defined(__linux__)
struct sigcontext *
#else
ucontext_t *
#endif
notify_get_signal_context(void);
void notify_enable_rpc_svc (int flag);

#endif
