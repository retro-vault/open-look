/*
 * Declares public XView interfaces, types, and attributes for
 * nintndeath.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(NINTNDEATH__H)
#define NINTNDEATH__H

#include <xview/pkg.h>
#include <xview_private/ntfy.h>

Notify_value notify_next_destroy_func(Notify_client nclient, Destroy_status status);

#endif
