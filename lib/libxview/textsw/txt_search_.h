/*
 * Declares internal interfaces used by the XView textsw module.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(TXT_SEARCH__H)
#define TXT_SEARCH__H

#include <xview/pkg.h>
#include <xview/panel.h>
#include <xview_private/txt_impl.h>

Pkg_private Es_index textsw_do_search_proc(Textsw_view_handle view, unsigned direction, unsigned ring_bell_status, int wrapping_off, int is_global);
Pkg_private void search_event_proc(Panel_item item, Event *event);
Pkg_private Panel textsw_create_search_panel(Frame frame, Textsw_view_handle view);

#endif
