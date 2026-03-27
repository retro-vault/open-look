/*
 * Declares private XView interfaces, types, and macros for text.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(TEXT__H)
#define TEXT__H

#include <xview/pkg.h>
#include <xview/text.h>
#include <xview/window.h>

Pkg_private int textsw_init(Xv_Window parent, Textsw textsw_public, Attr_attribute avlist[]);

#endif
