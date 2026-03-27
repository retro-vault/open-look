/*
 * Implements the pw btchrop routines used by the XView pw module.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#include <xview_private/pw_btchrop_.h>

Xv_public int
pw_batchrop(pw, x, y, op, sbp, n)
    Pixwin         *pw;
    int             x, y;
    int             op;
    struct pr_prpos sbp[];
    int             n;
{
    short           i;
    int             dest_x = x;
    int             dest_y = y;
    register struct pr_prpos *temp;

    for (i = 0; i < n; i++) {
	temp = &(sbp[i]);
	dest_x += temp->pos.x;
	dest_y += temp->pos.y;
	xv_rop((Xv_opaque)pw, dest_x, dest_y, temp->pr->pr_width,
	       temp->pr->pr_height, op, temp->pr, 0, 0);
    }
}
