/*
 * Implements the fmcmd dsty routines used by the XView frame module.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#include <xview_private/fmcmd_dsty_.h>
#include <xview_private/frame_cmd.h>

/* Destroy the frame struct */
Pkg_private int
frame_cmd_destroy(frame_public, status)
    Frame           frame_public;
    Destroy_status  status;
{
    Frame_cmd_info *frame = FRAME_CMD_PRIVATE(frame_public);

    if (status == DESTROY_CLEANUP) {
	free((char *) frame);
    }

    return XV_OK;
}
