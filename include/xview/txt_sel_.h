/*
 * Declares public XView interfaces, types, and attributes for txt sel.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(TXT_SEL__H)
#define TXT_SEL__H

#include <xview/pkg.h>
#include <xview_private/txt_impl.h>

#ifdef OW_I18N
Xv_public void textsw_normalize_view_wc(Textsw abstract, Es_index pos);
#endif
Xv_public void textsw_normalize_view(Textsw abstract, Es_index pos);
#ifdef OW_I18N
Xv_public void textsw_possibly_normalize_wc(Textsw abstract, Es_index pos);
#endif
Xv_public void textsw_possibly_normalize(Textsw abstract, Es_index pos);
Pkg_private void textsw_not_visible_normalize(Textsw abstract, Es_index pos);
Pkg_private void textsw_possibly_normalize_and_set_selection(Textsw abstract, Es_index first, Es_index last_plus_one, unsigned type);
Pkg_private void textsw_normalize_internal(register Textsw_view_handle view, Es_index first, Es_index last_plus_one, int upper_context, int lower_context, register unsigned flags);
Pkg_private Es_index textsw_set_insert(register Textsw_folio folio, register Es_index pos);
Pkg_private caddr_t textsw_checkpoint_undo(Textsw abstract, caddr_t undo_mark);
Pkg_private void textsw_checkpoint_again(Textsw abstract);
#ifdef OW_I18N
Xv_public void textsw_set_selection_wcs(Textsw abstract, Es_index first, Es_index last_plus_one, unsigned type);
#endif
Xv_public void textsw_set_selection(Textsw abstract, Es_index first, Es_index last_plus_one, unsigned type);
Pkg_private Textsw_view_handle textsw_view_for_entity_view(Textsw_folio folio, Ev_handle e_view);
Pkg_private int textsw_mouseless_select_event(register Textsw_view_handle view, register Event *ie, Notify_arg arg);
Pkg_private void textsw_start_seln_tracking(Textsw_view_handle view, register Event *ie);
Pkg_private Es_index textsw_do_balance_beam(Textsw_view_handle view, int x, int y, register Es_index first, register Es_index last_plus_one);
Pkg_private int textsw_track_selection(Textsw_view_handle view, register Event *ie);
Pkg_private int textsw_get_selection_as_int(register Textsw_folio folio, unsigned type, int default_value);
Pkg_private int textsw_get_selection_as_string(register Textsw_folio folio, unsigned type, CHAR *buf, int buf_max_len);

#endif
