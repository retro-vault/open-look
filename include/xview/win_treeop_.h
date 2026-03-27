/*
 * Declares public XView interfaces, types, and attributes for win
 * treeop.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(WIN_TREEOP__H)
#define WIN_TREEOP__H

#include <xview/pkg.h>
#include <xview/server.h>
#include <X11/Xresource.h>

Xv_public XID win_getlink(Xv_object window, int linkname);
Xv_public void win_setlink(Xv_object window, int linkname, XID number);
Xv_private void win_set_parent(Xv_object window, Xv_object parent, int x, int y);
void win_insert(Xv_object window);
void win_insert_in_front(Xv_object window);
void win_remove(Xv_object window);
Xv_private char *win_name_for_qualified_xid(char *name, Display *display, XID xid);
Xv_public Xv_object win_number_to_object(Xv_object window, XID number);
Xv_public XID win_nametonumber(char *name);
Xv_public void win_numbertoname(int winnumber, char *name);
Xv_public char *win_fdtoname(Xv_object window, char *name);
Xv_public XID win_fdtonumber(Xv_object window);
Xv_private void win_free(Xv_object window);
Xv_private Xv_object win_data(Display *display, XID xid);
Xv_private int win_is_mapped(Xv_object window);
Xv_private int win_view_state(Display *display, XID xid);
Xv_private void win_change_property(Xv_object window, Server_attr property_name, Atom property_type, int data_size, unsigned char *property_data, int data_count);
Xv_private void win_get_property(Xv_object window, Server_attr property_name, long data_offset, long data_count, Atom property_type, unsigned long *returned_data_count, unsigned long *bytes_after, unsigned char **property_data);

#endif
