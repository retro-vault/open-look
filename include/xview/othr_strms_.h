/*
 * Declares public XView interfaces, types, and attributes for othr
 * strms.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(OTHR_STRMS__H)
#define OTHR_STRMS__H

#include <xview/pkg.h>
#include <xview_private/io_stream.h>

STREAM *xv_filter_comments_stream(STREAM *in);

#endif
