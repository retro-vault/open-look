/*
 * Declares internal interfaces used by the XView icon module.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(ICON__H)
#define ICON__H

#include <xview/pkg.h>
#include <xview/icon.h>

Xv_private void icon_display(Icon icon_public, register int x, register int y);

#endif
