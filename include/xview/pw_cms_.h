/*
 * Declares public XView interfaces, types, and attributes for pw cms.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(PW_CMS__H)
#define PW_CMS__H

#include <xview/pkg.h>
#include <xview_private/pw_impl.h>
#include <xview/window.h>

Xv_public int pw_getcmsname(Xv_opaque pw, char *name);
Xv_public int pw_putcolormap(Xv_opaque pw, int index, int count, unsigned char red[], unsigned char green[], unsigned char blue[]);
Xv_public int pw_getcolormap(Xv_opaque pw, int index, int count, unsigned char red[], unsigned char green[], unsigned char blue[]);
Xv_public int pw_putattributes(Xv_opaque pw, register int *planes);
Xv_public int pw_getattributes(Xv_opaque pw, int *planes);

#endif
