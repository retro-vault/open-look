/*
 * Declares internal interfaces used by the XView scrollbar module.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(SB_SET__H)
#define SB_SET__H

#include <xview/pkg.h>
#include <xview_private/sb_impl.h>

Pkg_private Xv_opaque scrollbar_set_internal(Scrollbar scroll_public, Attr_avlist avlist);

static int scrollbar_parse_attr(Xv_scrollbar_info *sb, Attr_avlist argv);

#endif
