/*
 * Implements the om data routines used by the XView menu module.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#include <xview_private/om_get_.h>
#include <xview_private/om_public_.h>
#include <xview_private/om_set_.h>

Xv_pkg          xv_command_menu_pkg = {
    "Command Menu",		/* seal -> package name */
    (Attr_pkg) ATTR_PKG_MENU,	/* menu attr */
    sizeof(Xv_menu),		/* size of the menu public data structure */
    &xv_generic_pkg,		/* pointer to parent */
    menu_create_internal,	/* init routine */
    menu_sets,
    menu_gets,
    menu_destroy_internal,
    NULL			/* no find proc */
};

Xv_pkg          xv_choice_menu_pkg = {
    "Choice Menu",		/* seal -> package name */
    (Attr_pkg) ATTR_PKG_MENU,	/* menu attr */
    sizeof(Xv_menu),		/* size of the menu public data structure */
    &xv_generic_pkg,		/* pointer to parent */
    menu_create_internal,	/* init routine */
    menu_sets,
    menu_gets,
    menu_destroy_internal,
    NULL			/* no find proc */
};

Xv_pkg          xv_toggle_menu_pkg = {
    "Toggle Menu",		/* seal -> package name */
    (Attr_pkg) ATTR_PKG_MENU,	/* menu attr */
    sizeof(Xv_menu),		/* size of the menu public data structure */
    &xv_generic_pkg,		/* pointer to parent */
    menu_create_internal,	/* init routine */
    menu_sets,
    menu_gets,
    menu_destroy_internal,
    NULL			/* no find proc */
};

Xv_pkg          xv_menu_item_pkg = {
    "Menu_item",
    (Attr_pkg) ATTR_PKG_MENU,	/* menu item shares menu attrs */
    sizeof(Xv_menu_item),	/* size of the item public data structure */
    &xv_generic_pkg,
    menu_create_item_internal,
    menu_item_sets,
    menu_item_gets,
    menu_item_destroy_internal,
    menu_pkg_find
};
