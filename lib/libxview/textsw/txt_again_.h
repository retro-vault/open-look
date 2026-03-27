/*
 * Declares internal interfaces used by the XView textsw module.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(TXT_AGAIN__H)
#define TXT_AGAIN__H

#include <xview/pkg.h>
#include <xview_private/txt_impl.h>

Pkg_private int match_in_table(register char *to_match, register char **table); 
Pkg_private void textsw_record_caret_motion(Textsw_folio textsw, unsigned direction, int loc_x);
Pkg_private void textsw_record_delete(Textsw_folio textsw);
Pkg_private void textsw_record_edit(Textsw_folio textsw, unsigned unit, unsigned direction);
Pkg_private void textsw_record_extras(Textsw_folio folio, CHAR *cmd_line);
Pkg_private void textsw_record_find(Textsw_folio textsw, CHAR *pattern, int pattern_length, int direction);
Pkg_private void textsw_record_filter(Textsw_folio textsw, Event *event);
Pkg_private void textsw_record_input(Textsw_folio textsw, CHAR *buffer, long int buffer_length);
Pkg_private void textsw_record_match(Textsw_folio textsw, unsigned flag, CHAR *start_marker);
Pkg_private void textsw_record_piece_insert(Textsw_folio textsw, Es_handle pieces);
Pkg_private void textsw_record_trash_insert(Textsw_folio textsw);
Pkg_private void textsw_free_again(Textsw_folio textsw, register string_t *again);
Pkg_private int textsw_get_recorded_x(register Textsw_view_handle view);
Pkg_private void textsw_do_again(register Textsw_view_handle view, int x, int y);

#endif
