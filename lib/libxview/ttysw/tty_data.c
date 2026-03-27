/*
 * fix for shared libraries in SunOS4.0. Code was isolated from.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#include <sys/types.h>
#include <sys/time.h>
#include <xview_private/tty_.h>
#include <xview_private/term_impl.h>
#undef CTRL
#include <xview_private/ttyansi.h>


int             cursor = BLOCKCURSOR | LIGHTCURSOR;
int		tty_has_new_bufmod;			/* used to defeat 5.0
							   user-land pty
							   buffering hack
							   */


struct timeval  ttysw_bell_tv = {0, 100000};	/* 1/10 second */


Xv_pkg          xv_tty_pkg = {
    "Tty",
    (Attr_pkg) ATTR_PKG_TTY,
    sizeof(Xv_tty),
    &xv_openwin_pkg,
    tty_folio_init,
    ttysw_folio_set,
    ttysw_folio_get,
    ttysw_folio_destroy,
    NULL			/* no find proc */
};

Xv_pkg          xv_tty_view_pkg = {
    "Tty_view",
    (Attr_pkg) ATTR_PKG_TTY_VIEW,
    sizeof(Xv_tty_view),
    &xv_window_pkg,
    tty_view_init,
    ttysw_view_set,
    ttysw_view_get,
    ttysw_view_destroy,
    NULL			/* no find proc */
};
