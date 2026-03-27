/*
 * Declares internal interfaces used by the XView io_stream module.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(IO__H)
#define IO__H

#include <xview/pkg.h>
#include <xview_private/io_stream.h>


void stream_close(STREAM *stream);
struct posrec stream_get_pos(STREAM *stream);

#endif
