/*
 * Implements the output routines used by the XView io_stream module.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#include <xview_private/output_.h>
#include <xview_private/gettext_.h>
#include <xview_private/i18n_impl.h>
#include <xview/xv_error.h>

#define Get_Output_Ops \
	struct output_ops_vector *ops;\
	if (out->stream_type != Output) \
	    xv_error((Xv_object)NULL, \
		ERROR_SEVERITY, ERROR_NON_RECOVERABLE, \
		ERROR_STRING, \
		XV_MSG("output stream not of type output"), \
		NULL); \
	ops = out->ops.output_ops


static void stream_putstring(char *s, STREAM *out, Bool include_newline);

/* GENERIC OUTPUT FUNCTIONS */

int
stream_putc(c, out)
    char            c;
    STREAM         *out;
{
    Get_Output_Ops;
    return ((*ops->str_putc) (c, out));
}

void
stream_puts(s, out)
    char           *s;
    STREAM         *out;
{
    stream_putstring(s, out, True);
}

void
stream_fputs(s, out)
    char           *s;
    STREAM         *out;
{
    stream_putstring(s, out, False);
}

static void
stream_putstring(s, out, include_newline)
    char           *s;
    STREAM         *out;
    Bool            include_newline;
{
    int             i;

    Get_Output_Ops;
    if (ops->str_fputs != NULL)
	(*ops->str_fputs) (s, out);
    else
	for (i = 0; s[i] != '\0'; i++)
	    (*ops->str_putc) (s[i], out);

    if (include_newline)
	(*ops->str_putc) ('\n', out);

}


void
stream_flush(out)
    STREAM         *out;
{
    void            (*fn) ();

    Get_Output_Ops;
    fn = ops->flush;
    if (fn != NULL)
	(*fn) (out);
}
