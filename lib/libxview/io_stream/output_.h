/*
 * Declares internal interfaces used by the XView io_stream module.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(OUTPUT__H)
#define OUTPUT__H

#include <xview/pkg.h>
#include <xview_private/io_stream.h>

int stream_putc(char c, STREAM *out);
void stream_puts(char *s, STREAM *out);
void stream_fputs(char *s, STREAM *out);
void stream_flush(STREAM *out);

#endif
