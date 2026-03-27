/*
 * Declares private XView interfaces, types, and macros for cnvs set.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(CNVS_SET__H)
#define CNVS_SET__H

#include <xview/pkg.h>
#include <xview/base.h>
#include <xview/attr.h>
#include <xview_private/cnvs_impl.h>

Pkg_private Xv_opaque canvas_set_avlist(Canvas canvas_public, Attr_avlist avlist);

#endif
