/*
 * Declares public XView interfaces, types, and attributes for txt
 * match.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(TXT_MATCH__H)
#define TXT_MATCH__H

#include <xview/pkg.h>
#include <xview/panel.h>
#include <xview_private/txt_impl.h>

Pkg_private Panel textsw_create_match_panel(Frame frame, Textsw_view_handle view);

#endif
