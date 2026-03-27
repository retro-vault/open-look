/*
 * Declares internal interfaces used by the XView textsw module.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(TXT_MOVE__H)
#define TXT_MOVE__H

#include <xview/pkg.h>
#include <xview_private/txt_impl.h>

Pkg_private void textsw_save_selection(Textsw_folio folio);
Pkg_private int textsw_do_move(Textsw_view_handle view, int selection_is_local);
Pkg_private int textsw_end_quick_move(Textsw_view_handle view);
Pkg_private void textsw_track_move(Textsw_view_handle view, register Event *ie);
Pkg_private void textsw_finish_move(Textsw_view_handle view, register Event *ie);
Pkg_private void textsw_clear_move(Textsw_view_handle view);
Pkg_private void textsw_track_duplicate(Textsw_view_handle view, register Event *ie);
Pkg_private void textsw_finish_duplicate(Textsw_view_handle view, register Event *ie);
Pkg_private void textsw_clear_duplicate(Textsw_view_handle view);
Pkg_private void textsw_reset_cursor(Textsw_view_handle view);
Pkg_private void xv_do_move(Textsw_view_handle view, register Event *ie);
Pkg_private void textsw_do_duplicate(Textsw_view_handle view, register Event *ie);
Pkg_private int textsw_clean_up_move(Textsw_view_handle view, Es_index first, Es_index last_plus_one);
Pkg_private void textsw_do_drag_copy_move(Textsw_view_handle view, Event *ie, int is_copy);
Pkg_private void textsw_do_remote_drag_copy_move(register Textsw_view_handle view, Event *ie, short is_copy);
void DndReplyProc(Selection_requestor sel, Atom target, Atom type, Xv_opaque value, int length, int format);

#endif
