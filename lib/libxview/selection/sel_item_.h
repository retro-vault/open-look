/*
 * Declares internal interfaces used by the XView selection module.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(SEL_ITEM__H)
#define SEL_ITEM__H

#include <xview/pkg.h>
#include <xview_private/sel_impl.h>

Pkg_private int sel_item_init(Selection_owner parent, Selection_item sel_item_public, Attr_avlist avlist);
Pkg_private Xv_opaque sel_item_set_avlist(Selection_item sel_item_public, Attr_avlist avlist);
Pkg_private Xv_opaque sel_item_get_attr(Selection_item sel_item_public, int *status, Attr_attribute attr, va_list valist);
Pkg_private int sel_item_destroy(Selection_item sel_item_public, Destroy_status status);

#endif
