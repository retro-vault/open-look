/*
 * Declares private XView interfaces, types, and macros for txt edit.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(TXT_EDIT__H)
#define TXT_EDIT__H

#include <xview/pkg.h>
#include <xview_private/txt_impl.h>

Pkg_private Es_handle textsw_esh_for_span(Textsw_view_handle view, Es_index first, Es_index last_plus_one, Es_handle to_recycle);
Pkg_private int textsw_adjust_delete_span(register Textsw_folio folio, register Es_index *first, register Es_index *last_plus_one);
Pkg_private void textsw_esh_failed_msg(Textsw_view_handle view, char *preamble);
Pkg_private Es_index textsw_delete_span(Textsw_view_handle view, Es_index first, Es_index last_plus_one, register unsigned flags);
Pkg_private Es_index textsw_do_pending_delete(Textsw_view_handle view, unsigned type, int flags);
#ifdef OW_I18N
Xv_public Textsw_index textsw_delete_wcs(Textsw abstract, Es_index first, Es_index last_plus_one);
#endif
Xv_public Textsw_index textsw_delete(Textsw abstract, Es_index first, Es_index last_plus_one);
#ifdef OW_I18N
Xv_public Textsw_index textsw_erase_wcs(Textsw abstract, Es_index first, Es_index last_plus_one);
#endif
Xv_public Textsw_index textsw_erase(Textsw abstract, Es_index first, Es_index last_plus_one);
Pkg_private void textsw_insert_makes_visible(Textsw textsw);
#ifdef OW_I18N
Pkg_private Es_index textsw_do_edit(register Textsw_view_handle view, unsigned unit, unsigned direction, unsigned byte_return);
#else 
Pkg_private Es_index textsw_do_edit(register Textsw_view_handle view, unsigned unit, unsigned direction);
#endif 
#ifdef OW_I18N
Xv_public Textsw_index textsw_edit_wcs(Textsw abstract, register unsigned unit, register unsigned count, unsigned direction);
#endif
Xv_public Textsw_index textsw_edit(Textsw abstract, register unsigned unit, register unsigned count, unsigned direction);
Pkg_private void textsw_input_before(Textsw_view_handle view, Es_index *old_insert_pos, Es_index *old_length);
Pkg_private Es_index textsw_input_partial(Textsw_view_handle view, CHAR *buf, long int buf_len);
Pkg_private Es_index textsw_input_after(Textsw_view_handle view, Es_index old_insert_pos, Es_index old_length, int record);

#ifdef OW_I18N
static CHAR *memchr_wcs(CHAR *buf, char c, long int buf_len);
#endif 

Pkg_private Es_index textsw_do_input(Textsw_view_handle view, CHAR *buf, long buf_len, unsigned flag);
#ifdef OW_I18N
Xv_public Textsw_index textsw_insert_wcs(Textsw abstract, CHAR *buf, int buf_len);
#endif
Xv_public Textsw_index textsw_insert(Textsw abstract, CHAR *buf, int buf_len);
#ifdef OW_I18N
Xv_public Textsw_index textsw_replace_wcs(Textsw abstract, Es_index first, Es_index last_plus_one, CHAR *buf, long int buf_len);
#endif
Xv_public Textsw_index textsw_replace_bytes(Textsw abstract, Es_index first, Es_index last_plus_one, CHAR *buf, long int buf_len);
Xv_public Textsw_index textsw_replace(Textsw abstract, Es_index first, Es_index last_plus_one, CHAR *buf, long int buf_len);
#ifdef OW_I18N
pkg_private int textsw_mbstowcs_by_mblen(register CHAR *wstr, register char *str, int *byte_len, int *big_len_flag);
pkg_private int textsw_error_skip_mbstowcs(register CHAR *wstr, register char *str, int *byte_len, int limit_bytes);
#endif 

#endif
