/*
 * Declares internal interfaces used by the XView menu module.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(OM_SET__H)
#define OM_SET__H

#include <xview/pkg.h>
#include <xview_private/om_impl.h>

Pkg_private Xv_opaque menu_sets(Menu menu_public, register Attr_attribute *attrs);
Pkg_private Xv_opaque menu_item_sets(Menu_item menu_item_public, register Attr_attribute *attrs);
Pkg_private void menu_destroys(register Xv_menu_info *m, void (*destroy_proc)());
Pkg_private void menu_item_destroys(register Xv_menu_item_info *mi, void (*destroy_proc)());
Pkg_private void menu_set_pin_window(Xv_menu_info *m, Xv_opaque pin_window);
Xv_private void menu_set_acc_on_frame(Frame frame, Menu menu, Menu_item item, int set);
Xv_private void menu_set_key_qual(Menu menu, Menu_item item, int set, KeySym keysym, unsigned int modifiers, unsigned int diamond_modmask, char *qual_str);

#endif
