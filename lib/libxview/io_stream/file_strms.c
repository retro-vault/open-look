/*
 * Implements the file strms routines used by the XView io_stream
 * module.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#include <xview_private/file_strms_.h>
#include <xview_private/xv_.h>
#include <xview/xv_error.h>
#include <xview_private/io_stream.h>
#include <stdio.h>	/* fgetc() */

/* STREAM FROM FILE */

#define GetFISData struct xv_file_input_stream_data *data = (struct xv_file_input_stream_data*) in->client_data

struct xv_file_input_stream_data {
    FILE           *fp;
    int             lineno;
};

static void xv_file_input_stream_close(STREAM *in);
static int xv_file_input_stream_getc(STREAM *in);
static struct posrec xv_file_input_stream_get_pos(STREAM *in);
static int xv_file_input_stream_set_pos(STREAM *in, int n);
static int xv_file_input_stream_ungetc(char c, STREAM *in);
static char *xv_file_input_stream_fgets(char *s, int n, STREAM *in);
static int xv_file_input_stream_chars_avail(STREAM *in);
static void xv_file_output_stream_close(STREAM *out);
static void xv_file_output_stream_flush(STREAM *out);
static int xv_file_output_stream_putc(char c, STREAM *out);
static void xv_file_output_stream_fputs(char* s, STREAM *out);
static struct posrec xv_file_output_stream_get_pos(STREAM *out);

static void
xv_file_input_stream_close(in)
    STREAM         *in;
{
    GetFISData;
    (void) fclose(data->fp);
    free((char *) data);
}

static int
xv_file_input_stream_getc(in)
    STREAM         *in;
{
    char            c;
    GetFISData;

    c = fgetc(data->fp);
    if (c == '\n' && data->lineno != -1)
	data->lineno++;
    return (c);
}

static struct posrec
xv_file_input_stream_get_pos(in)
    STREAM         *in;
{
    GetFISData;
    struct posrec   p;

    p.charpos = ftell(data->fp);
    p.lineno = data->lineno;
    return (p);
}
static int
xv_file_input_stream_set_pos(in, n)
    STREAM         *in;
    int             n;
{
    GetFISData;

    (void) fseek(data->fp, (long) n, 0);
    data->lineno = -1;
    return (n);
}

static int
xv_file_input_stream_ungetc(c, in)
    char            c;
    STREAM         *in;
{
    GetFISData;
    if (c == '\n' && data->lineno != -1)
	data->lineno++;
    return (ungetc(c, data->fp));
}

static char    *
xv_file_input_stream_fgets(s, n, in)
    char           *s;
    int             n;
    STREAM         *in;
{
    GetFISData;
    if (fgets(s, n, data->fp) == NULL)
	return (NULL);
    if (data->lineno != -1 && s[strlen(s) - 1] == '\n')
	data->lineno++;
    return (s);
}

static int
xv_file_input_stream_chars_avail(in)
    STREAM         *in;
{
    int             pos, val;
    GetFISData;

    pos = ftell(data->fp);	/* where we are now */
    (void) fseek(data->fp, (long) 0, 2);
    val = (ftell(data->fp) - pos);	/* end of file */
    (void) fseek(data->fp, (long) pos, 0);
    return (val);
}

static struct input_ops_vector xv_file_input_stream_ops = {
    xv_file_input_stream_getc,
    xv_file_input_stream_ungetc,
    xv_file_input_stream_fgets,
    xv_file_input_stream_chars_avail,
    xv_file_input_stream_get_pos,
    xv_file_input_stream_set_pos,
    xv_file_input_stream_close
};

STREAM         *
xv_file_input_stream(s, fp)
    char           *s;
    FILE           *fp;
{
    STREAM         *value;
    struct xv_file_input_stream_data *data;

    if (fp == NULL) {
	if ((fp = fopen(s, "r")) == NULL)
	    return (NULL);
    }
    value = (STREAM *) xv_malloc(sizeof(STREAM));
    if (value == NULL) {	/* malloc can fail */
	xv_error((Xv_object)NULL,
		 ERROR_SEVERITY, ERROR_SYSTEM,
		 NULL);
	return ((STREAM *)NULL);
    }
    value->stream_type = Input;
    value->stream_class = "Input Stream From File";
    value->ops.input_ops = &xv_file_input_stream_ops;
    data = (struct xv_file_input_stream_data *) xv_malloc(
				     sizeof(struct xv_file_input_stream_data));
    if (data == NULL) {
	xv_error((Xv_object)NULL,
		 ERROR_SEVERITY, ERROR_SYSTEM,
		 NULL);
	return ((STREAM *)NULL);
    }
    data->fp = fp;
    data->lineno = 1;
    value->client_data = (caddr_t) data;
    return (value);
}

/* STREAM TO FILE */

#define GetFOSData struct xv_file_output_stream_data *data = (struct xv_file_output_stream_data*) out->client_data

struct xv_file_output_stream_data {
    FILE           *fp;
    int             lineno;
};

static void
xv_file_output_stream_close(out)
    STREAM         *out;
{
    GetFOSData;
    (void) fclose(data->fp);
    free((char *) data);
}

static void
xv_file_output_stream_flush(out)
    STREAM         *out;
{
    GetFOSData;
    (void) fflush(data->fp);
    free((char *) data);
}

static int
xv_file_output_stream_putc(c, out)
    char            c;
    STREAM         *out;
{
    GetFOSData;
    if (c == '\n' && data->lineno != -1)
	data->lineno++;
    return (fputc(c, data->fp));
}

static void
xv_file_output_stream_fputs(s, out)
    char           *s;
    STREAM         *out;
{
    GetFOSData;
    fputs(s, data->fp);
}

static struct posrec
xv_file_output_stream_get_pos(out)
    STREAM         *out;
{
    struct posrec   p;
    GetFOSData;

    p.charpos = ftell(data->fp);
    p.lineno = data->lineno;
    return (p);
}


static struct output_ops_vector xv_file_output_stream_ops = {
    xv_file_output_stream_putc,
    xv_file_output_stream_fputs,
    xv_file_output_stream_get_pos,
    xv_file_output_stream_flush,
    xv_file_output_stream_close
};


STREAM         *
xv_file_output_stream(s, fp, append)
    char           *s;
    FILE           *fp;
    Bool            append;
{
    STREAM         *value;
    struct xv_file_output_stream_data *data;

    if (fp == NULL) {
	fp = fopen(s, (append == True ? "a" : "w"));
	if (fp == NULL)
	    return (NULL);
    }
    value = (STREAM *) xv_malloc(sizeof(STREAM));
    if (value == NULL) {	/* malloc can fail */
	xv_error((Xv_object)NULL,
		 ERROR_SEVERITY, ERROR_SYSTEM,
		 NULL);
	return ((STREAM *)NULL);
    }
    value->stream_type = Output;
    value->stream_class = "Output Stream To File";
    value->ops.output_ops = &xv_file_output_stream_ops;
    data = (struct xv_file_output_stream_data *) xv_malloc(
				    sizeof(struct xv_file_output_stream_data));
    if (data == NULL) {
	xv_error((Xv_object)NULL,
		 ERROR_SEVERITY, ERROR_SYSTEM,
		 NULL);
	return ((STREAM *)NULL);
    }
    data->fp = fp;
    data->lineno = 1;
    value->client_data = (caddr_t) data;
    return (value);
}
