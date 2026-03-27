/*
 * Declares public XView interfaces, types, and attributes for file
 * strms.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(FILE_STRMS__H)
#define FILE_STRMS__H

#include <xview/pkg.h>
#include <xview_private/io_stream.h>


STREAM *xv_file_input_stream(char *s, FILE *fp);
STREAM *xv_file_output_stream(char *s, FILE *fp, Bool append);

#endif
