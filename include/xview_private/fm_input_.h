/*
 * Declares private XView interfaces, types, and macros for fm input.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(FM_INPUT__H)
#define FM_INPUT__H

#include <xview/pkg.h>
#include <xview/frame.h>

Pkg_private Notify_value frame_input(Frame frame_public, Event *event, Notify_arg arg, Notify_event_type type);
Pkg_private void frame_focus_win_event_proc(Xv_Window window, Event *event, Notify_arg arg);
Pkg_private Notify_value frame_footer_input(Xv_Window footer, Event *event, Notify_arg arg, Notify_event_type type);
Xv_private void frame_set_accept_default_focus(Frame frame_public, int flag);
#ifdef OW_I18N
Pkg_private Notify_value frame_IMstatus_input(Xv_Window IMstatus, Event *event, Notify_arg arg, Notify_event_type type);
#endif

#endif
