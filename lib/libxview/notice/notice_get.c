/*
 * Implements the notice get routines used by the XView notice module.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#include <xview_private/notice_get_.h>
#include <stdio.h>
#include <X11/Xlib.h>
#include <xview_private/draw_impl.h>
#include <xview_private/noticeimpl.h>
#include <xview_private/portable.h>
#include <xview/font.h>
#include <xview/frame.h>
#include <xview/panel.h>
#include <xview/win_input.h>
#include <xview/cms.h>

Pkg_private Xv_opaque
notice_get_attr(notice_public, status, attr, valist)
Xv_notice_struct	*notice_public;
int			*status;
Notice_attribute	attr;
va_list			valist;
{
    Notice_info	*notice = NOTICE_PRIVATE(notice_public);
/* Alpha compatibility, mbuck@debian.org */
#if 0
    Attr_avlist     avlist = (Attr_avlist) valist;
#endif
    Xv_opaque	v = (Xv_opaque)NULL;

    switch (attr)  {
    case NOTICE_LOCK_SCREEN:
        v = (Xv_opaque)notice->lock_screen;
    break;

    case NOTICE_BLOCK_THREAD:
        v = (Xv_opaque)notice->block_thread;
    break;

    case NOTICE_NO_BEEPING:
        v = (Xv_opaque)notice->dont_beep;
    break;

    case NOTICE_FONT:
        v = (Xv_opaque)notice->notice_font;
    break;

    case NOTICE_STATUS:
        v = (Xv_opaque)notice->result;
    break;

    case XV_SHOW:
        v = (Xv_opaque)notice->show;
    break;

    default:
	if (xv_check_bad_attr(&xv_notice_pkg, attr) == XV_ERROR)  {
	    *status = XV_ERROR;
	}
    break;
    }

    return((Xv_opaque) v);

}
