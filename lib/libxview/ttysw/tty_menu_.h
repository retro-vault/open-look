/*
 * Declares internal interfaces used by the XView ttysw module.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(TTY_MENU__H)
#define TTY_MENU__H

#include <xview/pkg.h>
#include <xview_private/tty_impl.h>
#include <xview_private/term_impl.h>

Pkg_private Menu ttysw_walkmenu(Tty ttysw_folio_public);
Pkg_private void ttysw_show_walkmenu(Tty_view anysw_view_public, Event *event);
Pkg_private void ttysw_set_menu(Termsw termsw_public);
void fit_termsw_panel_and_textsw(Frame frame, register Termsw_folio termsw_folio);
void ttysw_enable_editor(Menu cmd_menu, Menu_item cmd_item);
void ttysw_disable_editor(Menu cmd_menu, Menu_item cmd_item);
int ttysw_mode_action(Menu cmd_menu, Menu_item cmd_item);


#endif
