/*
 * Implements the pw traprop routines used by the XView pw module.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#include <xview_private/pw_traprop_.h>
#include <xview_private/gettext_.h>
#include <xview_private/i18n_impl.h>
#include <pixrect/traprop.h>

Xv_public int
pw_traprop(pw, dx, dy, trap, op, spr, sx, sy)
    register Xv_opaque pw;
    register int    dx, dy;
    struct pr_trap  trap;
    int             op;
    struct pixrect *spr;
    int             sx, sy;
{
    xv_error((Xv_object)NULL,
	     ERROR_STRING, 
	         XV_MSG("pw_traprop: not implemented"),
	     NULL);
	return PIX_ERR;
}
