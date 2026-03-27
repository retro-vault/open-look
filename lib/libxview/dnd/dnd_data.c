/*
 * Implements the dnd data routines used by the XView dnd module.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#include <xview_private/dnd_pblc_.h>

Xv_pkg		xv_dnd_pkg = {
    "Drag & Drop", ATTR_PKG_DND,
    sizeof(Xv_dnd_struct),
    &xv_sel_owner_pkg,
    dnd_init,
    dnd_set_avlist,
    dnd_get_attr,
    dnd_destroy,
    NULL		/* BUG: Need find */
};
