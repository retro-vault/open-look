/*
 * Declares private XView interfaces, types, and macros for txt caret.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(TXT_CARET__H)
#define TXT_CARET__H

#include <xview/pkg.h>
#include <xview_private/primal.h>
#include <xview_private/txt_impl.h>

Pkg_private Es_index textsw_move_backward_a_word(register Textsw_view_handle view, Es_index pos);
Pkg_private Es_index textsw_move_down_a_line(register Textsw_view_handle view, Es_index pos, Es_index file_length, int lt_index, struct rect rect);
Pkg_private Es_index textsw_move_forward_a_word(Textsw_view_handle view, Es_index pos, Es_index file_length);
Pkg_private Es_index textsw_move_to_word_end(Textsw_view_handle view, Es_index pos, Es_index file_length);
Pkg_private Es_index textsw_move_to_line_start(Textsw_view_handle view, Es_index pos);
pkg_private Es_index textsw_move_to_line_end(Textsw_view_handle view, Es_index pos, Es_index file_length);
pkg_private Es_index textsw_move_next_line_start(Textsw_view_handle view, Es_index pos, Es_index file_length);
pkg_private Es_index textsw_move_up_a_line(register Textsw_view_handle view, Es_index pos, Es_index file_length, int lt_index, struct rect rect);
pkg_private void textsw_move_caret(Textsw_view_handle view, Textsw_Caret_Direction direction);

#endif
