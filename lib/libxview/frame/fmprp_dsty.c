/*
 * Implements the fmprp dsty routines used by the XView frame module.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#include <xview_private/fm_impl.h>
#include <xview_private/fm_props.h>

static void     frame_props_free();

/* Destroy the frame struct */
Pkg_private int
frame_props_destroy(frame_public, status)
    Frame           frame_public;
    Destroy_status  status;
{
    Frame_props_info *frame = FRAME_PROPS_PRIVATE(frame_public);

    if (status == DESTROY_CLEANUP) {	/* waste of time if ...PROCESS_DEATH */
	xv_destroy(frame->panel);
	frame_props_free(frame);
    }
    return XV_OK;
}

/*
 * free the frame struct and all its resources.
 */
static void
frame_props_free(frame)
    Frame_props_info *frame;
{
    /* Free frame struct */
    free((char *) frame);
}
