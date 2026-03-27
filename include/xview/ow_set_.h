/*
 * Declares public XView interfaces, types, and attributes for ow set.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(OW_SET__H)
#define OW_SET__H

#include <xview/pkg.h>
#include <xview_private/ow_impl.h>

Pkg_private Xv_opaque openwin_set(Openwin owin_public, Attr_avlist avlist);

#endif
