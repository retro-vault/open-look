/*
 * Declares public XView interfaces, types, and attributes for ow
 * rescale.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(OW_RESCALE__H)
#define OW_RESCALE__H

#include <xview/pkg.h>
#include <xview_private/ow_impl.h>

Pkg_private void openwin_rescale(Openwin owin_public, int scale);

#endif
