/*
 * Declares private XView interfaces, types, and macros for svr set.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(SVR_SET__H)
#define SVR_SET__H

#include <xview/pkg.h>
#include <xview_private/svr_impl.h>


Pkg_private Xv_opaque server_set_avlist(Xv_Server server_public, Attr_attribute avlist[]);
Xv_private void server_set_timestamp(Xv_Server server_public, struct timeval *ev_time, unsigned long xtime);
Xv_private void server_set_fullscreen(Xv_Server server_public, int in_fullscreen);
Pkg_private Server_xid_list *server_xidnode_from_xid(Server_info *server, Xv_opaque xid);
Pkg_private Server_proc_list *server_procnode_from_id(Server_info *server, Xv_opaque pkg_id);
Pkg_private Server_mask_list *server_masknode_from_xidid(Server_info *server, Xv_opaque xid, Xv_opaque pkg_id);
Xv_private void server_do_xevent_callback(Server_info *server, Display *display, XEvent *xevent);
Xv_private void server_semantic_map_offset(Xv_server server_public, unsigned int modifiers, int *offset);

#endif
