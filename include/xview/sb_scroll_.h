/*
 * Declares public XView interfaces, types, and attributes for sb
 * scroll.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(SB_SCROLL__H)
#define SB_SCROLL__H

#include <xview/pkg.h>
#include <xview_private/sb_impl.h>

Pkg_private int scrollbar_scroll(Xv_scrollbar_info *sb, int pos, Scroll_motion motion);
Pkg_private int scrollbar_scroll_to_offset(Xv_scrollbar_info *sb, long unsigned view_start);
Xv_public void scrollbar_default_compute_scroll_proc(Scrollbar scroll_public, int pos, int length, Scroll_motion motion, unsigned long *offset, unsigned long *object_length);

#endif
