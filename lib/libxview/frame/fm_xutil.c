/*
 * Implements the fm xutil routines used by the XView frame module.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#include <xview_private/fm_impl.h>
#include <X11/Xatom.h>

/*
 * This call should be replaced by XSetWMhints when the X/NeWS server adds
 * the message field to the XWMHints.
 */
Xv_private void
frame_setwmhints(display, w, wmhints)
    Display        *display;
    XID             w;
    XWMHints       *wmhints;
{
    XChangeProperty(display, w, XA_WM_HINTS, XA_WM_HINTS, 32,
		    PropModeReplace, (unsigned char *) wmhints, sizeof(XWMHints));
}
