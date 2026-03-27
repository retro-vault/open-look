/*
 * Declares public XView interfaces, types, and attributes for txt
 * scroll.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(TXT_SCROLL__H)
#define TXT_SCROLL__H

#include <xview/pkg.h>
#include <xview_private/txt_impl.h>

Pkg_private void textsw_scroll(Scrollbar sb);
Pkg_private int textsw_compute_scroll(Scrollbar sb, int pos, int length, Scroll_motion motion, long unsigned *offset, long unsigned *obj_length);
Pkg_private int textsw_mouseless_scroll_event(register Textsw_view_handle view, register Event *ie, Notify_arg arg);
Pkg_private void textsw_real_update_scrollbars(register Textsw_folio folio);
Pkg_private void textsw_update_scrollbars(register Textsw_folio folio, register Textsw_view_handle only_view);
Pkg_private Scrollbar textsw_get_scrollbar(Textsw_view_handle view);

#endif
