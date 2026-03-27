/*
 * Declares internal interfaces used by the XView textsw module.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(EV_ONCE__H)
#define EV_ONCE__H

#include <xview/pkg.h>
#include <xview/window.h>
#include <xview_private/ev.h>

Pkg_private Ev_chain ev_create_chain(Es_handle esh, Ei_handle eih);
Pkg_private Ev_handle ev_create_view(Ev_chain chain, Xv_Window pw, struct rect *rect);
Pkg_private void ev_destroy(Ev_handle view);
Pkg_private Ev_chain ev_destroy_chain_and_views(Ev_chain chain);
Pkg_private Ev_handle ev_view_above(Ev_handle this_view);
Pkg_private Ev_handle ev_view_below(Ev_handle this_view);
Pkg_private Ev_handle ev_highest_view(Ev_chain chain);
Pkg_private Ev_handle ev_lowest_view(Ev_chain chain);
Pkg_private void ev_line_info(register Ev_handle view, int *top, int *bottom);
Pkg_private int ev_newlines_in_esh(Es_handle esh, Es_index first, Es_index last_plus_one);
Pkg_private int ev_rect_for_ith_physical_line(register Ev_handle view, int phys_line, register Es_index *first, Rect *rect, int skip_white_space);
Pkg_private Es_index ev_position_for_physical_line(Ev_chain chain, register int line, int skip_white_space);
Pkg_private void ev_find_in_esh(Es_handle esh, CHAR *pattern, int pattern_length, Es_index position, unsigned count, int flags, Es_index *first, Es_index *last_plus_one);

#endif
