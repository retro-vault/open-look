/*
 * Declares private XView interfaces, types, and macros for term ntfy.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(TERM_NTFY__H)
#define TERM_NTFY__H

#include <xview/pkg.h>
#include <xview/text.h>
#include <xview_private/tty_impl.h>
#include <xview_private/term_impl.h>

Pkg_private void ttysw_move_mark(Textsw textsw, Textsw_mark *mark, Textsw_index to, int flags);
Pkg_private Notify_value ttysw_text_event(register Textsw_view textsw, Event *event, Notify_arg arg, Notify_event_type type);
Pkg_private int ttysw_scan_for_completed_commands(Ttysw_view_handle ttysw_view, int start_from, int maybe_partial);
Pkg_private void ttysw_doing_pty_insert(register Textsw textsw, Termsw_folio commandsw, int toggle);
Pkg_private int ttysw_cooked_echo_cmd(Ttysw_view_handle ttysw_view, char *buf, int buflen);
Pkg_private void ttysw_textsw_changed(Textsw textsw, Attr_avlist attributes);

#endif
