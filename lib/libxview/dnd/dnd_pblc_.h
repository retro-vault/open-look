/*
 * Declares internal interfaces used by the XView dnd module.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(DND_PBLC__H)
#define DND_PBLC__H

#include <xview/pkg.h>
#include <xview_private/dndimpl.h>

Pkg_private int dnd_init(Xv_Window parent, Xv_drag_drop dnd_public, Attr_avlist avlist);
Pkg_private Xv_opaque dnd_set_avlist(Dnd dnd_public, Attr_attribute avlist[]);
Pkg_private Xv_opaque dnd_get_attr(Dnd dnd_public, int *status, Dnd_attribute attr, va_list args);
Pkg_private int dnd_destroy(Dnd dnd_public, Destroy_status status);

#endif
