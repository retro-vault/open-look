/*
 * Declares private XView interfaces, types, and macros for input.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(INPUT__H)
#define INPUT__H

#include <xview/pkg.h>
#include <xview/str_utils.h>
#include <xview_private/io_stream.h>

int stream_chars_avail(STREAM *in);
int stream_getc(STREAM *in);
int stream_peekc(STREAM *in);
int stream_ungetc(char c, STREAM *in);
char *stream_gets(char *s, STREAM *in);
char *stream_fgets(char *s, int n, STREAM *in);
char *stream_get_token(STREAM *in, char *dest, enum CharClass (*charproc)(char));
char *stream_get_sequence(STREAM *in, char *dest, struct CharAction (*charproc)(char));
void xv_skip_over(STREAM *in, enum CharClass (*charproc)(char));
int stream_set_pos(STREAM *in, int n);

#endif
