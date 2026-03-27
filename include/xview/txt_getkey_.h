/*
 * Declares public XView interfaces, types, and attributes for txt
 * getkey.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(TXT_GETKEY__H)
#define TXT_GETKEY__H

#include <xview/pkg.h>
#include <xview_private/txt_impl.h>

Pkg_private void textsw_begin_get(Textsw_view_handle view);
Pkg_private int textsw_end_get(Textsw_view_handle view);
Pkg_private Es_index textsw_read_only_boundary_is_at(register Textsw_folio folio);
Pkg_private Es_index textsw_insert_pieces(Textsw_view_handle view, register Es_index pos, Es_handle pieces);
Pkg_private int textsw_function_get(Textsw_view_handle view);
Pkg_private void textsw_put_then_get(Textsw_view_handle view);
Pkg_private Ev_mark_object textsw_add_mark_internal(Textsw_folio textsw, Es_index position, unsigned flags);
#ifdef OW_I18N
Xv_public Textsw_mark textsw_add_mark_wc(Textsw abstract, Es_index position, unsigned flags);
#endif
Xv_public Textsw_mark textsw_add_mark(Textsw abstract, Es_index position, unsigned flags);
Pkg_private Es_index textsw_find_mark_internal(Textsw_folio textsw, Ev_mark_object mark);
#ifdef OW_I18N
Xv_public Textsw_index textsw_find_mark_wc(Textsw abstract, Textsw_mark mark);
#endif
Xv_public Textsw_index textsw_find_mark(Textsw abstract, Textsw_mark mark);
Pkg_private void textsw_remove_mark_internal(Textsw_folio textsw, Ev_mark_object mark);
Xv_public void textsw_remove_mark(Textsw abstract, Textsw_mark mark);

#endif
