/*
 * Implements the fmhlp dsty routines used by the XView frame module.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#include <xview_private/fmhlp_dsty_.h>
#include <xview_private/fm_impl.h>
#include <xview_private/frame_help.h>

static void frame_help_free(Frame_help_info *frame);

/* Destroy the frame struct */
Pkg_private int
frame_help_destroy(frame_public, status)
    Frame           frame_public;
    Destroy_status  status;
{
    Frame_help_info *frame = FRAME_HELP_PRIVATE(frame_public);

    if (status == DESTROY_CLEANUP) {	/* waste of time if ...PROCESS_DEATH */
	frame_help_free(frame);
    }
    return XV_OK;
}

/*
 * free the frame struct and all its resources.
 */
static void
frame_help_free(frame)
    Frame_help_info *frame;
{
    /* Free frame struct */
    free((char *) frame);
}
