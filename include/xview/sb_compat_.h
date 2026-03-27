/*
 * Declares public XView interfaces, types, and attributes for sb
 * compat.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(SB_COMPAT__H)
#define SB_COMPAT__H

#include <xview/pkg.h>
#include <xview_private/sb_impl.h>

Sv1_public Scrollbar _scrollbar_create(Attr_attribute attr1, ...);
Sv1_public int _scrollbar_set(Scrollbar sb_public, ...);
Sv1_public Xv_opaque scrollbar_get(Scrollbar sb_public, Attr_attribute attr);
Sv1_public int scrollbar_destroy(Scrollbar sb_public);
Sv1_public void scrollbar_scroll_to(Scrollbar sb_public, unsigned long new_start);

#endif
