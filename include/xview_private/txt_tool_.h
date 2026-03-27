/*
 * Declares private XView interfaces, types, and macros for txt tool.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(TXT_TOOL__H)
#define TXT_TOOL__H

#include <xview/pkg.h>
#include <xview_private/txt_impl.h>

Pkg_private void textsw_default_notify(Textsw abstract, Attr_attribute *attrs);

#endif
