/*
 * Declares public XView interfaces, types, and attributes for sel req.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(SEL_REQ__H)
#define SEL_REQ__H

#include <xview/pkg.h>
#include <xview/window.h>
#include <xview_private/sel_impl.h>

Pkg_private int sel_req_init(Xv_Window parent, Selection_requestor sel_req_public, Attr_avlist avlist);
Pkg_private Xv_opaque sel_req_set_avlist(Selection_requestor sel_req_public, Attr_avlist avlist);
Pkg_private Xv_opaque sel_req_get_attr(Selection_requestor sel_req_public, int *status, Attr_attribute attr, va_list valist);
Pkg_private int sel_req_destroy(Selection_requestor sel_req_public, Destroy_status status);
int sel_post_req(Selection_requestor sel);
Xv_private int xv_sel_handle_selection_notify(XSelectionEvent *ev);
Xv_private int xv_sel_handle_property_notify(XPropertyEvent *ev);

#endif
