/*
 * Declares internal interfaces used by the XView textsw module.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(TXT_ATTR__H)
#define TXT_ATTR__H

#include <xview/pkg.h>
#include <xview_private/txt_impl.h>

Pkg_private void textsw_set_null_view_avlist(register Textsw_folio folio, Attr_avlist attrs);
Pkg_private Textsw_status textsw_set_internal(register Textsw_folio textsw, Textsw_view_handle view, Attr_attribute *attrs, int is_folio);
Pkg_private void textsw_set_internal_tier2(register Textsw_folio textsw, Textsw_view_handle view, Attr_attribute *attrs, int is_folio, Textsw_status *status_ptr, char **error_msg_addr, CHAR *file, CHAR *name_wc, int *reset_mode, int *all_views, int *update_scrollbar, int *read_only_changed);
Pkg_private void textsw_view_cms_change(register Textsw_folio textsw, Textsw_view_handle view);
Pkg_private long textsw_get_from_defaults(register Textsw_attribute attribute);
Pkg_private Xv_opaque textsw_get(Textsw abstract, int *status, Textsw_attribute attribute, va_list args);
Pkg_private Xv_opaque textsw_view_get(Textsw_view view_public, int *status, Textsw_attribute attribute, va_list args);
Pkg_private Xv_opaque textsw_set(Textsw abstract, Textsw_attribute avlist[]);
Pkg_private Xv_opaque textsw_view_set(Textsw_view view_public, Textsw_attribute avlist[]);
Pkg_private void _textsw_notify(Textsw_view_handle view, ...);
Pkg_private void textsw_notify_replaced(Textsw_opaque folio_or_view, Es_index insert_before, Es_index length_before, Es_index replaced_from, Es_index replaced_to, Es_index count_inserted);
#ifdef OW_I18N
Pkg_private Es_index textsw_get_contents_wcs(register Textsw_folio textsw, Es_index position, CHAR *buffer, register int buffer_length);
#endif
Pkg_private Es_index textsw_get_contents(register Textsw_folio textsw, Es_index position, CHAR *buffer, register int buffer_length);

#endif
