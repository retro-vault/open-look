/*
 * Declares public XView interfaces, types, and attributes for termsw.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(TERMSW__H)
#define TERMSW__H

#include <xview/pkg.h>
#include <xview_private/term_impl.h>

Pkg_private int termsw_folio_init(Xv_Window parent, Termsw termsw_public, Textsw_attribute avlist[]);
Pkg_private Xv_opaque termsw_folio_set(Termsw_view termsw_folio_public, Attr_avlist avlist);
Pkg_private Xv_opaque termsw_folio_get(Termsw_view termsw_folio_public, int *status, Termsw_attribute attr, va_list args);
Pkg_private int termsw_folio_destroy(Termsw_view termsw_folio_public, Destroy_status status);
Pkg_private int termsw_view_init(Xv_Window parent, Termsw_view termsw_view_public, Termsw_attribute avlist[]);
Pkg_private Xv_opaque termsw_view_set(Termsw_view termsw_view_public, Attr_avlist avlist);
Pkg_private Xv_opaque termsw_view_get(Termsw_view termsw_view_public, int *status, Attr_attribute attr, va_list args);
Pkg_private int termsw_view_destroy(Termsw_view termsw_view_public, Destroy_status status);
Pkg_private void termsw_menu_set(void);
Pkg_private void termsw_menu_clr(void);
Pkg_private void termsw_caret_cleared(void);
void termsw_caret_rendered(void);
short termsw_caret_invalid(void);

#endif
