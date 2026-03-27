/*
 * Declares public XView interfaces, types, and attributes for ndet
 * dodis.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(NDET_DODIS__H)
#define NDET_DODIS__H

#include <xview/pkg.h>
#include <xview_private/ntfy.h>

Notify_error notify_do_dispatch(void);

#endif
