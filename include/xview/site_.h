/*
 * Declares public XView interfaces, types, and attributes for site.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(SITE__H)
#define SITE__H

#include <xview/pkg.h>
#include <xview_private/dndimpl.h>
#include <xview_private/site_impl.h>

Pkg_private Xv_opaque DndDropAreaOps(register Dnd_site_info *site, register Dnd_region_ops mode, register Xv_opaque area);
Pkg_private void DndSizeOfSite(register Dnd_site_info *site);
Xv_private int DndStoreSiteData(register Xv_drop_site site_public, register long **prop);

#endif
