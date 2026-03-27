/*
 * Declares internal interfaces used by the XView sel module.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(SEL_COMPAT__H)
#define SEL_COMPAT__H

#include <xview/pkg.h>
#include <xview_private/seln_impl.h>

Xv_public char *seln_create(void (*function_proc)(), Seln_result (*request_proc)(), char *client_data);
Xv_public void seln_destroy(char *client);
Xv_public void seln_report_event(Seln_client seln_client, struct inputevent *event);
Xv_public Seln_holder seln_inquire(Seln_rank which);
Xv_public void seln_clear_functions(void);
Xv_public Seln_rank seln_acquire(Seln_client seln_client, Seln_rank asked);
Xv_public Seln_result seln_request(Seln_holder *holder, Seln_request *buffer);
Xv_public Seln_holders_all seln_inquire_all(void);
Xv_public Seln_result seln_done(Seln_client seln_client, Seln_rank rank);
Xv_public Seln_function_buffer seln_inform(Seln_client seln_client, Seln_function which, int down);
Seln_response seln_figure_response(Seln_function_buffer *buffer, Seln_holder **holder);
Xv_public Seln_result seln_hold_file(Seln_rank rank, char *path);
Xv_public void seln_yield_all(void);
Xv_public void seln_use_timeout(int seconds);

#endif
