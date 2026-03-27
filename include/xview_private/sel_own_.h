/*
 * Declares private XView interfaces, types, and macros for sel own.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(SEL_OWN__H)
#define SEL_OWN__H

#include <xview/pkg.h>
#include <xview/window.h>
#include <xview_private/sel_impl.h>

Pkg_private int sel_owner_init(Xv_Window parent, Selection_owner sel_owner_public, Attr_avlist avlist);
Pkg_private Xv_opaque sel_owner_set_avlist(Selection_owner sel_owner_public, Attr_avlist avlist);
Pkg_private Xv_opaque sel_owner_get_attr(Selection_owner sel_owner_public, int *status, Attr_attribute attr, va_list valist);
Pkg_private int sel_owner_destroy(Selection_owner sel_owner_public, Destroy_status status);
Xv_public Bool sel_convert_proc(Selection_owner sel_owner_public, Atom *type, Xv_opaque *data, unsigned long *length, int *format);
Xv_private int xv_sel_handle_selection_request(XSelectionRequestEvent *reqEvent);
Xv_private int xv_sel_handle_incr(Sel_owner_info *selection);
Xv_private int xv_sel_handle_selection_clear(XSelectionClearEvent *clrEv);

#endif
