/*
 * Declares internal interfaces used by the XView ttysw module.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(TTY__H)
#define TTY__H

#include <xview/pkg.h>
#include <xview/tty.h>
#include <xview/window.h>
#include <xview_private/term_impl.h>

Pkg_private int tty_folio_init(Xv_Window parent, Tty tty_public, Tty_attribute avlist[]);
Pkg_private int tty_view_init(Xv_Window parent, Tty_view tty_view_public, Tty_attribute avlist[]);
Pkg_private Xv_opaque ttysw_folio_set(Tty ttysw_folio_public, Tty_attribute avlist[]);
Pkg_private Xv_opaque ttysw_view_set(Tty_view ttysw_view_public, Tty_attribute avlist[]);
Pkg_private Xv_opaque ttysw_folio_get(Tty ttysw_folio_public, int *status, Tty_attribute attr, va_list args);
Pkg_private Xv_opaque ttysw_view_get(Tty_view ttysw_view_public, int *status, Tty_attribute attr, va_list args);
Pkg_private int ttysw_view_destroy(Tty_view ttysw_view_public, Destroy_status status);
Pkg_private int ttysw_folio_destroy(Ttysw_folio ttysw_folio_public, Destroy_status status);

#endif
