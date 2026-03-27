/*
 * Declares public XView interfaces, types, and attributes for txt
 * disp.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(TXT_DISP__H)
#define TXT_DISP__H

#include <xview/pkg.h>
#include <xview_private/txt_impl.h>

Pkg_private void textsw_display(Textsw abstract);
Pkg_private void textsw_display_view(Textsw abstract, register Rect *rect);
Pkg_private void textsw_display_view_margins(register Textsw_view_handle view, struct rect *rect);
Pkg_private void textsw_repaint(register Textsw_view_handle view);
Pkg_private void textsw_resize(register Textsw_view_handle view);
Pkg_private void textsw_do_resize(Textsw abstract);
Pkg_private Textsw_expand_status textsw_expand(Textsw abstract, Es_index start, Es_index stop_plus_one, CHAR *out_buf, int out_buf_len, int *total_chars);

#endif
