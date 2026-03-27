/*
 * Implements the base data routines used by the XView base module.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#include <xview_private/generic_.h>
#include <xview_private/drawable_.h>
#include <xview/generic.h>
Xv_pkg          xv_generic_pkg = {
    "Generic",
    ATTR_PKG_GENERIC,
    sizeof(Xv_generic_struct),
    NULL,			/* No parent package */
    generic_init,
    generic_set_avlist,
    generic_get,
    generic_destroy,
    NULL			/* No find procedure */
};

Xv_pkg          xv_drawable_pkg = {
    "Drawable",
    ATTR_PKG_DRAWABLE,
    sizeof(Xv_drawable_struct),
    &xv_generic_pkg,
    drawable_init,
    NULL,			/* No set allowed */
    drawable_get_attr,
    drawable_destroy,
    NULL,			/* No find procedure */
};


/* if 0; ignore Key-press/mouse-press synthetic events */
int  defeat_event_security; 
