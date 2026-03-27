/*
 * Declares internal interfaces used by the XView ttysw module.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(TTY_NTFY__H)
#define TTY_NTFY__H

#include <xview/pkg.h>
#include <xview/textsw.h>
#include <xview_private/tty_impl.h>

Pkg_private void ttysw_interpose(Ttysw_folio ttysw_folio);
Pkg_private void ttysw_interpose_on_textsw(Textsw_view textsw);
Pkg_private int ttysw_destroy(Tty ttysw_folio_public, Destroy_status status);
Pkg_private void ttysw_sigwinch(Ttysw_folio ttysw);
Pkg_private void ttysw_sendsig(Ttysw_folio ttysw, Textsw textsw, int sig);
Xv_public Notify_value ttysw_event(Tty_view ttysw_view_public, Event *event, Notify_arg arg, Notify_event_type type);
Pkg_private void ttysw_display(Ttysw_folio ttysw, Event *ie);
Pkg_private Notify_value ttysw_pty_input_pending(Tty tty_public, int pty);
Pkg_private Notify_value ttysw_itimer_expired(Tty tty_public, int which);
Pkg_private void ttysw_reset_conditions(Ttysw_view_handle ttysw_view);
Pkg_private void ttysw_resize(Ttysw_view_handle ttysw_view);
void cim_resize(Ttysw_view_handle ttysw_view);
Pkg_private void csr_resize(Ttysw_view_handle ttysw_view);

#endif
