/*
 * Declares private XView interfaces, types, and macros for sel policy.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(SEL_POLICY__H)
#define SEL_POLICY__H

#include <xview/pkg.h>
#include <xview_private/seln_impl.h>

Xv_public Seln_response selection_figure_response(Xv_Server server, Seln_function_buffer *buffer, Seln_holder **holder);
Xv_public void selection_report_event(Xv_Server server, Seln_client seln_client, struct inputevent *event);

#endif
