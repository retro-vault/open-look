/*
 * Declares private XView interfaces, types, and macros for tty modes.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(TTY_MODE__H)
#define TTY_MODE__H

#include <xview/pkg.h>
#include <xview_private/tty_impl.h>

Pkg_private int ttysw_be_ttysw(Ttysw_view_handle ttysw_view);
Pkg_private int ttysw_be_termsw(Ttysw_view_handle ttysw_view);
Pkg_private void ttysw_getp(Ttysw_view_handle ttysw_view);
void ttysw_cooked_echo(Ttysw_view_handle ttysw_view, int old_cooked_echo, int new_cooked_echo);

#endif
