/*
 * Declares private XView interfaces, types, and macros for cim size.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(CIM_SIZE__H)
#define CIM_SIZE__H

#include <xview/pkg.h>
#include <xview_private/tty_impl.h>

Pkg_private int xv_tty_imageinit(Ttysw *ttysw, Xv_object window);
Pkg_private void xv_tty_imagealloc(Ttysw *ttysw, int for_temp);
Pkg_private void xv_tty_free_image_and_mode(void);
Pkg_private void ttysw_imagerepair(Ttysw_view_handle ttysw_view);

#endif
