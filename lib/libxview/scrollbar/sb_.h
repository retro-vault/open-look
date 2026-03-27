/*
 * Declares internal interfaces used by the XView scrollbar module.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(SB__H)
#define SB__H

#include <xview/pkg.h>
#include <xview_private/sb_impl.h>

Pkg_private int scrollbar_create_internal(Xv_opaque parent, Xv_opaque sb_public, Xv_opaque *avlist);
Pkg_private void scrollbar_create_standard_menu(Xv_scrollbar_info *sb);

#endif
