/*
 * Declares internal interfaces used by the XView textsw module.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(FINGER_TBL__H)
#define FINGER_TBL__H

#include <xview/pkg.h>
#include <xview_private/finger_tbl.h>

Pkg_private void ft_add_delta(ft_object finger_table, int from, long int delta);
Pkg_private ft_object ft_create(int last_plus_one, int sizeof_client_data);
Pkg_private void ft_destroy(ft_handle table);
Pkg_private void ft_expand(register ft_handle table, int by);
Pkg_private void ft_shift_up(register ft_handle table, int first, int last_plus_one, int expand_by);
Pkg_private void ft_shift_out(ft_handle table, int first, int last_plus_one);
Pkg_private void ft_set(ft_object finger_table, int first, int last_plus_one, Es_index to, char *client_data);
Pkg_private void ft_set_esi_span(ft_object finger_table, Es_index first, Es_index last_plus_one, Es_index to, char *client_data);
Pkg_private int ft_bounding_index(register Ft_table finger_table, Es_index pos);
Pkg_private int ft_index_for_position(ft_object finger_table, Es_index pos);
Pkg_private Es_index ft_position_for_index(ft_object finger_table, int index);
#ifdef DEBUG
Pkg_private int fprintf_ft(ft_object finger_table);
#endif

#endif
