/*
 * Implements the cms data routines used by the XView color module.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#include <xview_private/cms_.h>
#include <xview_private/cms_pblc_.h>
#include <xview_private/cms_impl.h>

Xv_pkg          xv_cms_pkg = {
    "Color", ATTR_PKG_CMS,
    sizeof(Xv_cms_struct),
    &xv_generic_pkg,
    cms_init,
    cms_set_avlist,
    cms_get_attr,
    cms_destroy,
    cms_find_cms    
};
