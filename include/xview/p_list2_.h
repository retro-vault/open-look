/*
 * Declares public XView interfaces, types, and attributes for p list2.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(P_LIST2__H)
#define P_LIST2__H

#include <xview/pkg.h>
#include <xview_private/panel_impl.h>

Pkg_private void panel_list_row_inactive_set(Panel_list_row_values *entry, int value);
Pkg_private int panel_list_row_inactive_get(Panel_list_row_values *entry);

#endif
