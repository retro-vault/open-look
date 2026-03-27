/*
 * Implements the drawable routines used by the XView base module.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#include <xview_private/drawable_.h>
#include <xview_private/windowutil_.h>
#include <xview_private/xv_.h>
#include <xview_private/win_info.h>		/* window_private_gc() */
#define _NOTIFY_MIN_SYMBOLS
#include <xview/notify.h>
#undef _NOTIFY_MIN_SYMBOLS

const char *xv_draw_info_str = "drawable_info";
const char *xv_notptr_str = "not a pointer";

/*ARGSUSED*/
Pkg_private int
drawable_init(parent_public, drawable_public, avlist, offset_ptr)
    Xv_opaque       parent_public;
    Xv_drawable_struct *drawable_public;
    Attr_avlist     avlist;
    int            *offset_ptr;
{
    drawable_public->private_data = (Xv_opaque) xv_alloc(Xv_Drawable_info);
    return XV_OK;
}

Pkg_private int
drawable_destroy(drawable_public, status)
    register Xv_Drawable drawable_public;
    Destroy_status  status;
{
    register        Xv_Drawable_info
    *               drawable = DRAWABLE_PRIVATE(drawable_public);

    if (status == DESTROY_CLEANUP) {
	(void) free((char *) drawable);
	return XV_OK;
    }
    return XV_OK;
}

Pkg_private Xv_opaque
drawable_get_attr(drawable_public, status, attr)
    Xv_Drawable     drawable_public;
    int            *status;
    Drawable_attr   attr;
{

    Xv_Drawable_info *info;

    switch (attr) {
      case DRAWABLE_INFO:
	return ((Xv_opaque) DRAWABLE_PRIVATE(drawable_public));

      case XV_XID:
	info = DRAWABLE_PRIVATE(drawable_public);
	return ((Xv_opaque) (info->xid));

      case XV_DISPLAY:
	info = DRAWABLE_PRIVATE(drawable_public);
	return ((Xv_opaque) (info->visual->display));

      default:
	if (xv_check_bad_attr(&xv_drawable_pkg,
				     (Attr_attribute) attr) == XV_ERROR) {
	    *status = XV_ERROR;
	}
	return (XV_NULL);
    }
}

Xv_private GC
xv_private_gc(d)
    Xv_opaque       d;
{
    return ((GC) window_private_gc(d));
}
