/*
 * Declares internal interfaces used by the XView sel module.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(SEL_CLNT__H)
#define SEL_CLNT__H

#include <xview/pkg.h>
#include <xview_private/seln_impl.h>

Xv_public Seln_client selection_create(Xv_Server server, void (*function_proc)(), Seln_result(*request_proc)(), char *client_data);
Xv_public void selection_destroy(Xv_Server server, char *client);
Xv_public Seln_rank selection_acquire(Xv_Server server, Seln_client seln_client, Seln_rank asked);
Xv_public Seln_result selection_done(Xv_Server server, Seln_client seln_client, Seln_rank rank);
Xv_public void selection_yield_all(Xv_Server server);
Xv_public Seln_function_buffer selection_inform(Xv_Server server, Seln_client seln_client, Seln_function which, int down);
Xv_public Seln_holder selection_inquire(Xv_Server server, Seln_rank which);
Xv_public Seln_holders_all selection_inquire_all(Xv_Server server);
Xv_public void selection_clear_functions(Xv_Server server);
Xv_public Seln_result selection_request(Xv_Server server, Seln_holder *holder, Seln_request *buffer);
Pkg_private void selection_init_holder_info(Xv_Server server, Seln_rank rank);
Pkg_private void seln_give_up_selection_without_telling_server(Xv_Server server, Seln_rank rank);
Xv_public Seln_result selection_hold_file(Xv_Server server, Seln_rank rank, char *path);
Xv_public void selection_use_timeout(Xv_Server server, int seconds);

#endif
