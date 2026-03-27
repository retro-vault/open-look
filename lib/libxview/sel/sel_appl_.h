/*
 * Declares internal interfaces used by the XView sel module.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(SEL_APPL__H)
#define SEL_APPL__H

#include <xview/pkg.h>
#include <xview_private/seln_impl.h>

Xv_public Seln_request *_seln_ask(Seln_holder *holder, ...);
Xv_public Seln_request *_selection_ask(Xv_Server server, Seln_holder *holder, ...);
Xv_public void _seln_init_request(Seln_request *buffer, Seln_holder *holder, ...);
Xv_public void _selection_init_request(Xv_Server server, Seln_request *buffer, Seln_holder *holder, ...);
Xv_public Seln_result _seln_query(Seln_holder *holder, Seln_result (*reader)(Seln_request *), char *context, ...);
Xv_public Seln_result _selection_query(Xv_Server server, Seln_holder *holder, Seln_result (*reader)(Seln_request *), char *context, ...);

#endif
