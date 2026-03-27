/*
 * Implements the svrim data routines used by the XView server_image
 * module.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#include <sys/types.h>
#include <stdio.h>
#include <pixrect/pixrect.h>
#ifdef __STDC__ 
#ifndef CAT
#define CAT(a,b)        a ## b 
#endif 
#endif
#include <pixrect/memvar.h>

#include <xview_private/svrim_impl.h>
#include <xview_private/svrim_pblc_.h>

Xv_pkg          xv_server_image_pkg = {
    "Server Image",			/* seal -> package name */
    ATTR_PKG_IMAGE,		/* image attr */
    sizeof(Xv_server_image),	/* size of the image data struct */
    &xv_drawable_pkg,		/* pointer to parent */
    server_image_create_internal,	/* init routine for icon */
    server_image_set_internal,	/* set routine */
    server_image_get_internal,	/* get routine */
    server_image_destroy_internal,	/* destroy routine */
    NULL			/* no find proc */
};
