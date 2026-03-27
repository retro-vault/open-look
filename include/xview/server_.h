/*
 * Declares public XView interfaces, types, and attributes for server.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(SERVER__H)
#define SERVER__H

#include <xview/pkg.h>
#include <xview_private/svr_atom.h>
#include <xview_private/svr_impl.h>


Pkg_private int server_init(Xv_opaque parent, Xv_Screen server_public, Attr_avlist avlist);
Pkg_private int server_destroy(Xv_Server server_public, Destroy_status status);
Xv_private Server_atom_type server_get_atom_type(Xv_Server server_public, Atom atom);
Xv_private void server_journal_sync_event(Xv_Server server_public, int type);
Xv_private void xv_string_to_rgb(char *buffer, unsigned char *red, unsigned char *green, unsigned char *blue);

#endif
