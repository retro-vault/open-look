/*
 * Declares internal interfaces used by the XView sel module.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(SEL_COMMON__H)
#define SEL_COMMON__H

#include <xview/pkg.h>
#include <xview_private/seln_impl.h>

Xv_public int seln_holder_same_client(register Seln_holder *holder, register char *client_data);
Xv_public int seln_holder_same_process(register Seln_holder *holder);
Xv_public int seln_same_holder(register Seln_holder *h1, register Seln_holder *h2);
Pkg_private int seln_equal_access(register Seln_access *first, register Seln_access *second);
Pkg_private int selection_equal_agent(Xv_Server server, Seln_holder holder);
Pkg_private Xv_opaque selection_agent_client(Xv_Server server);
Xv_public int seln_secondary_made(Seln_function_buffer *buffer);
Xv_public int seln_secondary_exists(Seln_function_buffer *buffer);
Pkg_private Seln_result selection_send_yield_internal(Xv_Server server, Seln_rank rank, Seln_holder *holder);
Pkg_private Seln_result selection_send_yield(Xv_Server server, Seln_rank rank, Seln_holder *holder);
Pkg_private void seln_init_reply(Seln_request *request, Seln_request *reply, Seln_replier_data *replier);
Pkg_private Seln_result seln_get_reply_buffer(Seln_request *buffer);
Pkg_private Seln_rank selection_to_rank(Atom selection, Seln_agent_info *agent);
Pkg_private Atom seln_rank_to_selection(Seln_rank rank, Seln_agent_info *agent);

#endif
