/*
 * Declares public XView interfaces, types, and attributes for p num
 * txt.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(P_NUM_TXT__H)
#define P_NUM_TXT__H

#include <xview/pkg.h>
#include <xview_private/panel_impl.h>

Pkg_private int panel_num_text_init(Panel panel_public, Panel_item item_public, Attr_avlist avlist);
Pkg_private Xv_opaque panel_num_text_set_avlist(Panel_item item_public, Attr_avlist avlist);
Pkg_private Xv_opaque panel_num_text_get_attr(Panel_item item_public, int *status, register Attr_attribute which_attr, va_list valist);
Pkg_private int panel_num_text_destroy(Panel_item item_public, Destroy_status status);

#endif
