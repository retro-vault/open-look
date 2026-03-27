/*
 * Implements the icon data routines used by the XView icon module.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#include <xview_private/icon_obj_.h>

Xv_pkg          xv_icon_pkg = {
    "Icon",			/* seal -> package name */
    (Attr_pkg) ATTR_PKG_ICON,	/* icon attr */
    sizeof(Xv_icon),		/* size of the icon data struct */
    &xv_window_pkg,		/* pointer to parent */
    icon_init,			/* init routine for icon */
    icon_set_internal,		/* set routine */
    icon_get_internal,		/* get routine */
    icon_destroy_internal,	/* destroy routine */
    NULL
};
