/*
 * Declares private XView interfaces, types, and macros for dnd dsdm.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(DND_DSDM__H)
#define DND_DSDM__H

#include <xview/pkg.h>
#include <X11/Xresource.h>
#include <xview_private/dndimpl.h>

Xv_private int DndContactDSDM(Dnd_info *dnd);
Xv_private int DndFindSite(Dnd_info *dnd, XButtonEvent *e);

#endif
