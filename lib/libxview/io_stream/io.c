/*
 * Implements the io routines used by the XView io_stream module.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#include <xview_private/io_.h>
#include <xview_private/gettext_.h>
#include <xview_private/i18n_impl.h>
#include <xview/xv_error.h>
#include <xview_private/io_stream.h>

/* GENERIC FUNCTIONS THAT APPLY TO BOTH INPUT AND OUTPUT */

void
stream_close(stream)
    STREAM         *stream;
{
    switch (stream->stream_type) {
      case Input:{
	    struct input_ops_vector *ops = stream->ops.input_ops;
	    (*(ops->close)) (stream);
	    goto out;
	}

      case Output:{
	    struct output_ops_vector *ops = stream->ops.output_ops;
	    (*(ops->close)) (stream);
	    goto out;
	}

      default:
	xv_error((Xv_opaque)stream,
		 ERROR_SEVERITY, ERROR_NON_RECOVERABLE,
		 ERROR_STRING, 
		 XV_MSG("invalid stream type"),
		 0);
    }
out:free((char *) stream);	/* client should have freed the client data */
}

struct posrec
stream_get_pos(stream)
    STREAM         *stream;
{
    switch (stream->stream_type) {
      case Input:{
	    struct input_ops_vector *ops = stream->ops.input_ops;
	    return ((*ops->get_pos) (stream));
	}
      case Output:{
	    struct output_ops_vector *ops = stream->ops.output_ops;
	    return ((*ops->get_pos) (stream));
	}
      default:{
	  struct posrec null_posrec;
	  null_posrec.lineno = -1;
	  null_posrec.charpos = -1;
	  xv_error((Xv_object)stream,
		   ERROR_SEVERITY, ERROR_NON_RECOVERABLE,
		   ERROR_STRING, 
		   XV_MSG("invalid stream type"), 
		   NULL);
	  return (null_posrec);
        }
    }
}
