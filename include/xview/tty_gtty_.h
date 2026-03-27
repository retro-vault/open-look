/*
 * Declares public XView interfaces, types, and attributes for tty
 * gtty.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(TTY_GTTY__H)
#define TTY_GTTY__H

#include <xview/pkg.h>
#include <xview_private/tty_impl.h>

Pkg_private int ttysw_restoreparms(int ttyfd);
#ifdef XV_USE_TERMIOS
int we_getptyparms(struct termios *tp);
#else
int we_getptyparms(int *ldisc, int *localmodes, struct sgttyb *mode, struct tchars *tchars, struct ltchars *ltchars);
#endif
Pkg_private int tty_getmode(int fd, tty_mode_t *mode);

#endif
