/*
 * Declares internal interfaces used by the XView color module.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(CMS_PBLC__H)
#define CMS_PBLC__H

#include <xview/pkg.h>
#include <X11/Xresource.h>
#include <xview_private/cms_impl.h>

Pkg_private int cms_init(Xv_Screen parent, Cms cms_public, Attr_avlist avlist);
Pkg_private Xv_opaque cms_set_avlist(Cms cms_public, Attr_attribute avlist[]);
Pkg_private Xv_opaque cms_get_attr(Cms cms_public, int *status, Cms_attribute attr, va_list args);
Pkg_private Xv_object cms_find_cms(Xv_opaque screen_public, Xv_pkg *pkg, Attr_avlist avlist);
Pkg_private int cms_destroy(Cms cms_public, Destroy_status status);

#endif
