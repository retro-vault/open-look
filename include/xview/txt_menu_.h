/*
 * Declares public XView interfaces, types, and attributes for txt
 * menu.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(TXT_MENU__H)
#define TXT_MENU__H

#include <xview/pkg.h>
#include <xview_private/txt_impl.h>

Pkg_private Menu textsw_menu_init(Textsw_folio folio);
Pkg_private Menu textsw_get_unique_menu(Textsw_folio folio);
Pkg_private void textsw_do_menu(Textsw_view_handle view, Event *ie);
Pkg_private Textsw_view textsw_from_menu(Menu menu);
Pkg_private void textsw_file_do_menu_action(Menu cmd_menu, Menu_item cmd_item);
Pkg_private void textsw_set_extend_to_edge(Textsw_view_handle view, int height, int width);
Pkg_private void textsw_get_extend_to_edge(Textsw_view_handle view, int *height, int *width);
Pkg_private void textsw_set_file_menu_default_to_savefile(void);
Pkg_private void textsw_do_save(Textsw abstract, Textsw_folio textsw, Textsw_view_handle view);

#endif
