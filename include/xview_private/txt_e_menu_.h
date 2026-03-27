/*
 * Declares private XView interfaces, types, and macros for txt e menu.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(TXT_E_MENU__H)
#define TXT_E_MENU__H

#include <xview/pkg.h>
#include <xview_private/txt_impl.h>

Pkg_private char *textsw_get_extras_filename(Menu_item mi);
Menu_item textsw_extras_gen_proc(Menu_item mi, Menu_generate operation);
Pkg_private int textsw_build_extras_menu_items(Textsw_view textsw_view, char *file, Menu menu);
Pkg_private Menu_item textsw_handle_extras_menuitem(Menu menu, Menu_item item);
Pkg_private char **textsw_string_to_argv(char *command_line);


static char *check_filename_locale(char *locale, char *filename, int copy);
static int extras_menufile_changed(void);
static void textsw_remove_all_menu_items(Menu menu);
static int walk_getmenu(Textsw_view textsw_view, Menu m, char *file, FILE *mfd, int *lineno);
static void free_argv(char **argv);
static char *textsw_savestr(register char *s);
static char *textsw_save2str(register char *s, register char *t);
static int any_shell_meta(register char *s);

#endif
