/*
 * Declares public XView interfaces, types, and attributes for
 * ndetitimer.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(NDETITIMER__H)
#define NDETITIMER__H

#include <xview/pkg.h>
#include <xview_private/ntfy.h>

pkg_private int ndet_check_which(int which, NTFY_TYPE *type_ptr);
pkg_private int ndet_check_tv(struct timeval *tv);
pkg_private struct timeval ndet_tv_subt(struct timeval atv, struct timeval btv);
pkg_private struct timeval ndet_tv_min(struct timeval atv, struct timeval btv);
pkg_private struct timeval ndet_real_min(NTFY_ITIMER *ntfy_itimer, struct timeval current_tv);
pkg_private struct timeval ndet_virtual_min(NTFY_ITIMER *ntfy_itimer, struct timeval current_tv);
pkg_private int ndet_itimer_expired(NTFY_CLIENT *client, NTFY_CONDITION *condition);
pkg_private void ndet_reset_itimer_set_tv(NTFY_CONDITION *condition);

#endif
