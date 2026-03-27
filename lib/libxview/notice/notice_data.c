/*
 * Implements the notice data routines used by the XView notice module.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#include <xview_private/notice_.h>
#include <xview_private/notice_dstry_.h>
#include <xview_private/notice_get_.h>
#include <xview_private/notice_set_.h>
#include <xview_private/noticeimpl.h>

Xv_pkg          xv_notice_pkg = {
    "Notice", ATTR_PKG_NOTICE,
    sizeof(Xv_notice_struct),
    &xv_generic_pkg,		/* subclass of generic */
    notice_init_internal,
    notice_set_avlist,
    notice_get_attr,
    notice_destroy_internal,
    NULL			/* no find proc */
};
