/*
 * Declares public XView interfaces, types, and attributes for ev op
 * bdry.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(EV_OP_BDRY__H)
#define EV_OP_BDRY__H

#include <xview/pkg.h>
#include <xview_private/ev_impl.h>

Pkg_private Ev_finger_handle ev_add_finger(Ev_finger_table *fingers, register Es_index pos, opaque_t client_data, Ev_mark id_handle);
Ev_finger_handle ev_find_finger(Ev_finger_table *fingers, Ev_mark_object mark);
Pkg_private void ev_remove_finger(Ev_finger_table *fingers, Ev_mark_object mark);
Pkg_private Op_bdry_handle ev_add_op_bdry(Ev_finger_table *op_bdry, Es_index pos, unsigned type, Ev_mark mark);
Pkg_private Op_bdry_handle ev_find_op_bdry(Ev_finger_table op_bdry, Ev_mark_object mark);
Pkg_private unsigned ev_op_bdry_info_merge(Ev_finger_table op_bdry, register int i, int *next_i, unsigned prior);
Pkg_private unsigned ev_op_bdry_info(Ev_finger_table op_bdry, Es_index pos, int *next_i);
Pkg_private void ev_remove_op_bdry(Ev_finger_table *op_bdry, register Es_index pos, unsigned type, register unsigned mask);
#ifdef OW_I18N
Pkg_private void ev_remove_all_op_bdry(Ev_chain chain, register Es_index start, register Es_index end, unsigned type, register unsigned mask);
#endif
Pkg_private Ev_mark_object ev_add_glyph(register Ev_chain chain, register Es_index line_start, register Es_index pos, Pixrect *pr, int op, int offset_x, int offset_y, register int flags);
Pkg_private Ev_mark_object ev_add_glyph_on_line(register Ev_chain chain, int line, Pixrect *pr, int op, int offset_x, int offset_y, register int flags);
Pkg_private Op_bdry_handle ev_find_glyph(Ev_chain chain, Es_index line_start);
Pkg_private void ev_remove_glyph(Ev_chain chain, Ev_mark_object mark, unsigned flags);
Pkg_private void ev_set_glyph_pr(Ev_chain chain, Ev_mark_object mark, struct pixrect *pr);

#endif
