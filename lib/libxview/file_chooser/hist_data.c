/*
 * Implements the hist data routines used by the XView file_chooser
 * module.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#include <xview_private/hist_list_.h>
#include <xview_private/hist_menu_.h>

Xv_pkg history_list_pkg = {
    "History List",
    ATTR_PKG_HIST,
    sizeof(History_list_public),
    XV_GENERIC_OBJECT,
    hist_list_init,
    hist_list_set,
    hist_list_get,
    hist_list_destroy,
    hist_list_find,	/* yes, a find method! */
};



Xv_pkg history_menu_pkg = {
    "History Menu",
    ATTR_PKG_HIST,
    sizeof(History_menu_public),
    XV_GENERIC_OBJECT,
    hist_menu_init,
    hist_menu_set,
    hist_menu_get,
    hist_menu_destroy,
    NULL			/* no find */
};
