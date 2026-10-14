/*
 * Implements the mem ops routines used by the XView pw module.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#include <xview_private/mem_.h>
#include <xview_private/gettext_.h>
#include <sys/types.h>
#include <pixrect/pixrect.h>
#include <xview_private/i18n_impl.h>
#include <xview/xv_error.h>

static int xv_pr_error(Pixrect *pr);

/*
 * "Pixrect" operations vector
 */
struct pixrectops mem_ops = {
    xv_pr_error,		/* mem_rop, */
    xv_pr_error,		/* mem_stencil, */
    xv_pr_error,		/* mem_batchrop, */
    0,
    xv_mem_destroy,
    xv_pr_error,		/* mem_get, */
    xv_pr_error,		/* mem_put, */
    xv_pr_error,		/* mem_vector, */
    (Pixrect * (*) ()) xv_pr_error,	/* mem_region, */
    xv_pr_error,		/* mem_putcolormap, */
    xv_pr_error,		/* mem_getcolormap, */
    xv_pr_error,		/* mem_putattributes, */
    xv_pr_error,		/* mem_getattributes */
};


static int
xv_pr_error(pr)
    Pixrect        *pr;
{
    xv_error((Xv_opaque)pr,
	     ERROR_SEVERITY, ERROR_NON_RECOVERABLE,
	     ERROR_STRING, 
	         XV_MSG("Unsupported pixrect operation attempted"),
	     NULL);
    /* doesn't return */
	return PIX_ERR;
}
