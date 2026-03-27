/*
 * Declares private XView interfaces, types, and macros for txt incl.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(TXT_INCL__H)
#define TXT_INCL__H

#include <xview/pkg.h>
#include <xview/panel.h>
#include <xview_private/txt_impl.h>

Pkg_private Panel textsw_create_include_panel(Frame frame, Textsw_view_handle view);
Pkg_private int include_cmd_proc(Frame fc, CHAR *path, CHAR *file, Xv_opaque client_data);

#endif
