/*
 * Declares public XView interfaces, types, and attributes for txt
 * event.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(TXT_EVENT__H)
#define TXT_EVENT__H

#include <xview/pkg.h>
#include <xview_private/txt_impl.h>

Pkg_private Textsw_folio textsw_folio_for_view(Textsw_view_handle view);
Pkg_private Textsw_view_handle textsw_view_abs_to_rep(Textsw abstract);
Pkg_private Textsw textsw_view_rep_to_abs(Textsw_view_handle rep);
Pkg_private Notify_value textsw_event(Textsw_view view_public, register Event *event, Notify_arg arg, Notify_event_type type);
Pkg_private void textsw_stablize(register Textsw_folio folio, int blink);
Pkg_private int textsw_mouseless_misc_event(register Textsw_view_handle view, register Event *event);
Pkg_private Notify_value textsw_timer_expired(register Textsw_folio folio, int which);
Pkg_private void textsw_add_timer(register Textsw_folio folio, register struct timeval *timeout);
Pkg_private void textsw_remove_timer(register Textsw_folio folio);
Pkg_private void textsw_take_down_caret(register Textsw_folio textsw);
Pkg_private Notify_value textsw_blink(register Textsw_folio folio, int which);
Pkg_private void textsw_start_blinker(register Textsw_folio folio);
Pkg_private void textsw_stop_blinker(register Textsw_folio folio);
Pkg_private void textsw_show_caret(register Textsw_folio textsw);
Pkg_private void textsw_hide_caret(register Textsw_folio textsw);
Pkg_private void textsw_freeze_caret(register Textsw_folio textsw);
Pkg_private void textsw_thaw_caret(register Textsw_folio textsw);
Pkg_private void textsw_invert_caret(register Textsw_folio textsw);
Pkg_private int textsw_is_typing_pending(register Textsw_folio folio, Event *event);

#endif
