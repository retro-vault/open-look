/*
 * Declares private XView interfaces, types, and macros for svr x.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(SVR_X__H)
#define SVR_X__H

#include <xview/pkg.h>
#include <xview_private/svr_impl.h>

Pkg_private Xv_opaque server_init_x(char *server_name);
Xv_private void server_refresh_modifiers(Xv_opaque server_public, Bool update_map);
Xv_private void server_set_seln_function_pending(Xv_Server server_public, int flag);
Xv_private int server_get_seln_function_pending(Xv_Server server_public);

#endif
