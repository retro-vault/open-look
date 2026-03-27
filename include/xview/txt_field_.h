/*
 * Declares public XView interfaces, types, and attributes for txt
 * field.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(TXT_FIELD__H)
#define TXT_FIELD__H

#include <xview/pkg.h>
#include <xview_private/txt_impl.h>

Pkg_private void textsw_begin_match_field(register Textsw_view_handle view);
Pkg_private int textsw_end_match_field(register Textsw_view_handle view, int event_code, int x, int y);
Pkg_private Es_index textsw_match_same_marker(Textsw_folio folio, CHAR *marker, int marker_len, Es_index start_pos, unsigned direction);
Pkg_private void textsw_match_field(Textsw_folio textsw, Es_index *first, Es_index *last_plus_one, CHAR *symbol1, int symbol1_len, CHAR *symbol2, int symbol2_len, unsigned field_flag, int do_search);
Pkg_private int textsw_match_field_and_normalize(Textsw_view_handle view, register Es_index *first, register Es_index *last_plus_one, CHAR *buf1, int buf_len1, unsigned field_flag, int do_search);
Pkg_private int textsw_match_selection_and_normalize(register Textsw_view_handle view, CHAR *start_marker, register unsigned field_flag);
#ifdef OW_I18N
Xv_public int textsw_match_wcs(Textsw abstract, Textsw_index *first, Textsw_index *last_plus_one, CHAR *start_sym, int start_sym_len, CHAR *end_sym, int end_sym_len, unsigned field_flag);
#endif
Xv_public int textsw_match_bytes(Textsw abstract, Textsw_index *first, Textsw_index *last_plus_one, CHAR *start_sym, int start_sym_len, CHAR *end_sym, int end_sym_len, unsigned field_flag);

#endif
