/*
 * Declares internal interfaces used by the XView textsw module.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(TXT_ONCE__H)
#define TXT_ONCE__H

#include <xview/pkg.h>
#include <xview_private/txt_impl.h>

Pkg_private void textsw_init_again(register Textsw_folio folio, register int count);
Pkg_private void textsw_init_undo(register Textsw_folio folio, register int count);
Pkg_private void textsw_set_cursor(Textsw textsw, int cursor_type);
Pkg_private Textsw_view_handle textsw_view_init_internal(register Textsw_view_handle view, Textsw_status *status);
Pkg_private Textsw_folio textsw_init_internal(Textsw_folio folio, Textsw_status *status, void (*default_notify_proc)(Textsw, Attr_attribute*), Textsw_attribute *attrs);
Pkg_private void textsw_setup_scrollbar(Scrollbar sb);
Pkg_private int textsw_folio_destroy(Textsw folio_public, register Destroy_status status);
Pkg_private int textsw_view_destroy(Textsw_view view_public, register Destroy_status status);
Xv_private void textsw_register_view(Textsw textsw, Xv_Window view_public);
Xv_private void textsw_unregister_view(Textsw textsw, Xv_Window view_public);

#endif
