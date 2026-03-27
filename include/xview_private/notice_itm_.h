/*
 * Declares private XView interfaces, types, and macros for notice itm.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(NOTICE_ITM__H)
#define NOTICE_ITM__H

#include <xview/pkg.h>
#include <xview_private/noticeimpl.h>


Pkg_private int notice_create_base(Notice_info *notice);

#endif
