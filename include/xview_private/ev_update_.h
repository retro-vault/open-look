/*
 * Declares private XView interfaces, types, and macros for ev update.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(EV_UPDATE__H)
#define EV_UPDATE__H

#include <xview/pkg.h>
#include <xview_private/ev.h>

Pkg_private Es_index ev_lt_delta(Ev_handle view, Es_index before_edit, Es_index delta);
Pkg_private void ev_lt_format(Ev_handle view, Ev_line_table *new_lt, Ev_line_table *old_lt);
Pkg_private void ev_display_line(Ev_handle view, int width_before, int lt_index, Es_index first, Es_index last_plus_one);
Pkg_private void ev_lt_paint(Ev_handle view, Ev_line_table *new_lt, Ev_line_table *old_lt);

#endif
