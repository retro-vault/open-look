/*
 * Declares private XView interfaces, types, and macros for notice
 * dstry.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(NOTICE_DSTRY__H)
#define NOTICE_DSTRY__H

#include <xview/pkg.h>
#include <xview/notice.h>

Pkg_private int notice_destroy_internal(Xv_notice_struct *notice_public, Destroy_status status);

#endif
