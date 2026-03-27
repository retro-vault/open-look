/*
 * Declares internal interfaces used by the XView panel module.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(P_SET__H)
#define P_SET__H

#include <xview/pkg.h>
#include <xview_private/panel_impl.h>

Pkg_private Xv_opaque panel_set_avlist(Panel panel_public, register Attr_avlist avlist);
Pkg_private void panel_refont(register Panel_info *panel, int arg);

#endif
