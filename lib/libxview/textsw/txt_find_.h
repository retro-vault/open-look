/*
 * Declares internal interfaces used by the XView textsw module.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(TXT_FIND__H)
#define TXT_FIND__H

#include <xview/pkg.h>
#include <xview_private/txt_impl.h>

Pkg_private void textsw_begin_find(register Textsw_view_handle view);
Pkg_private int textsw_end_find(register Textsw_view_handle view, unsigned event_code, int x, int y);
Pkg_private void textsw_find_selection_and_normalize(register Textsw_view_handle view, int x, int y, register long unsigned options);
Pkg_private void textsw_find_pattern(Textsw_folio textsw, Es_index *first, Es_index *last_plus_one, CHAR *buf, unsigned buf_len, unsigned flags);
Pkg_private void textsw_find_pattern_and_normalize(Textsw_view_handle view, int x, int y, Es_index *first, Es_index *last_plus_one, CHAR *buf, unsigned buf_len, unsigned flags);
Pkg_private int textsw_function_find(Textsw_view_handle view, int x, int y);
#ifdef OW_I18N
Xv_public int textsw_find_wcs(Textsw abstract, Es_index *first, Es_index *last_plus_one, CHAR *buf, unsigned buf_len, unsigned flags);
#endif
Xv_public int textsw_find_bytes(Textsw abstract, Es_index *first, Es_index *last_plus_one, CHAR *buf, unsigned buf_len, unsigned flags);

#endif
