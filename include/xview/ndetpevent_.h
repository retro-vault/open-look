/*
 * Declares public XView interfaces, types, and attributes for
 * ndetpevent.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(NDETPEVENT__H)
#define NDETPEVENT__H

#include <xview/pkg.h>
#include <xview_private/ntfy.h>


Notify_error notify_post_event(Notify_client nclient, Notify_event event, Notify_event_type when_hint);
Notify_error notify_post_event_and_arg(Notify_client nclient, Notify_event event, Notify_event_type when_hint, Notify_arg arg, Notify_copy copy_func, Notify_release  release_func);
pkg_private void ndet_set_event_processing(Notify_client nclient, int on);

#endif
