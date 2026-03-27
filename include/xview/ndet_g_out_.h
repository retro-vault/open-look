/*
 * Declares public XView interfaces, types, and attributes for ndet g
 * out.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(NDET_G_OUT__H)
#define NDET_G_OUT__H

#include <xview/pkg.h>
#include <xview_private/ntfy.h>


Notify_func notify_get_output_func(Notify_client nclient, int fd);

#endif
