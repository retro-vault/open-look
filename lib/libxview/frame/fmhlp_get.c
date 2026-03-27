/*
 * Implements the fmhlp get routines used by the XView frame module.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#include <xview_private/fmhlp_get_.h>
#include <xview_private/fm_impl.h>
#include <xview_private/frame_help.h>

/* ARGSUSED */
Pkg_private     Xv_opaque
frame_help_get_attr(frame_public, status, attr, valist)
    Frame           frame_public;
    int            *status;
    Frame_attribute attr;
    va_list         valist;
{
    register Frame_help_info *frame = FRAME_HELP_PRIVATE(frame_public);

    switch (attr) {

      case FRAME_SHOW_LABEL:
	attr = (Frame_attribute) ATTR_NOP(attr);
	return (Xv_opaque) status_get(frame, show_label);

      default:
	*status = XV_ERROR;
	return (Xv_opaque) 0;
    }
}
