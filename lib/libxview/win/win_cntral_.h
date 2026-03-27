/*
 * Declares internal interfaces used by the XView win module.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(WIN_CNTRAL__H)
#define WIN_CNTRAL__H

#include <xview/pkg.h>
#include <xview/win_input.h>

Notify_error win_post_id(Notify_client client, int id, Notify_event_type when);
Notify_error win_post_id_and_arg(Notify_client client, int id, Notify_event_type when, Notify_arg arg, Notify_copy copy_func, Notify_release release_func);
Notify_error win_post_event(Notify_client client, Event *event, Notify_event_type when);
Notify_error win_post_event_arg(Notify_client client, Event *event, Notify_event_type when, Notify_arg arg, Notify_copy copy_func, Notify_release release_func);
Notify_arg win_copy_event(Notify_client client, Notify_arg arg, Event **event_ptr);
void win_free_event(Notify_client client, Notify_arg arg, Event *event);

#endif
