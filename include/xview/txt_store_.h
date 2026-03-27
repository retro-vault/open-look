/*
 * Declares public XView interfaces, types, and attributes for txt
 * store.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(TXT_STORE__H)
#define TXT_STORE__H

#include <xview/pkg.h>
#include <xview/panel.h>
#include <xview_private/txt_impl.h>

Pkg_private Panel textsw_create_store_panel(Frame frame, Textsw_view_handle view);

#endif
