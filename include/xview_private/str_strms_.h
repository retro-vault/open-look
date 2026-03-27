/*
 * Declares private XView interfaces, types, and macros for str strms.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(STR_STRMS__H)
#define STR_STRMS__H

#include <xview/pkg.h>
#include <xview_private/io_stream.h>


STREAM *string_input_stream(char *s, STREAM *in);
STREAM * string_output_stream(char *s, STREAM *out);

#endif
