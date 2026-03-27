/*
 * Implements the fmprp get routines used by the XView frame module.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#include <xview_private/fm_impl.h>
#include <xview_private/fm_props.h>

Pkg_private     Xv_opaque
frame_props_get_attr(frame_public, status, attr, valist)
    Frame           frame_public;
    int            *status;
    Frame_attribute attr;
    va_list         valist;
{
    register Frame_props_info *frame = FRAME_PROPS_PRIVATE(frame_public);

    switch (attr) {

      case FRAME_SHOW_LABEL:
	attr = (Frame_attribute) ATTR_NOP(attr);
	return (Xv_opaque) status_get(frame, show_label);

      case FRAME_SHOW_FOOTER:
	attr = (Frame_attribute) ATTR_NOP(attr);
	return (Xv_opaque) status_get(frame, show_footer);

      case FRAME_PROPS_PUSHPIN_IN:
	attr = (Frame_attribute) ATTR_NOP(attr);
	return (Xv_opaque) status_get(frame, pushpin_in);

      case FRAME_SCALE_STATE:
	attr = (Frame_attribute) ATTR_NOP(attr);
	/*
	 * WAIT FOR NAYEEM return (Xv_opaque)
	 * window_get_rescale_state(frame_public);
	 */
	return (Xv_opaque) 0;

      case FRAME_PROPS_PANEL:
	return (Xv_opaque) frame->panel;

      default:
	*status = XV_ERROR;
	return (Xv_opaque) 0;
    }
}
