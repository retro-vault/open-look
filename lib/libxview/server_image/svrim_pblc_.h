/*
 * Declares internal interfaces used by the XView server_image module.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(SVRIM_PBLC__H)
#define SVRIM_PBLC__H

#include <xview/pkg.h>
#include <xview_private/svrim_impl.h>

Pkg_private int server_image_create_internal(Xv_opaque parent, Server_image server_image_public, Attr_avlist avlist, int *offset_ptr);
Pkg_private Xv_opaque server_image_set_internal(Server_image server_image_public, Attr_attribute *avlist);
Pkg_private Xv_opaque server_image_get_internal(Server_image server_image_public, int *status, Server_image_attribute attr, va_list args);
Pkg_private int server_image_destroy_internal(Server_image server_image_public, Destroy_status status);
Pkg_private int server_image_destroy(Pixrect *pr);

#endif
