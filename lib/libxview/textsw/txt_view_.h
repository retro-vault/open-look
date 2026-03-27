/*
 * Declares internal interfaces used by the XView textsw module.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(TXT_VIEW__H)
#define TXT_VIEW__H

#include <xview/pkg.h>
#include <xview_private/txt_impl.h>

Pkg_private int textsw_view_init(Textsw parent, Textsw_view textsw_view_public, Attr_attribute avlist[]);
Pkg_private void textsw_split_init_proc(Textsw_view public_view, Textsw_view public_new_view, int position);

#endif
