/*
 * Implements the sb data routines used by the XView scrollbar module.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#include <xview_private/sb_.h>
#include <xview_private/sb_destroy_.h>
#include <xview_private/sb_get_.h>
#include <xview_private/sb_set_.h>

/*
 * Global Defines:
 */
Xv_pkg          xv_scrollbar_pkg = {
    "Scrollbar",			/* seal -> package name */
    (Attr_pkg) ATTR_PKG_SCROLLBAR,	/* scrollbar attr */
    sizeof(Xv_scrollbar),		/* size of the scrollbar data struct */
    &xv_window_pkg,			/* pointer to parent */
    scrollbar_create_internal,		/* init routine for scrollbar */
    scrollbar_set_internal,		/* set routine */
    scrollbar_get_internal,		/* get routine */
    scrollbar_destroy_internal,		/* destroy routine */
    NULL				/* No find proc */
};
