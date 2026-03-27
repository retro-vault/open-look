/*
 * Declares private XView interfaces, types, and macros for xv olgx.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(XV_OLGX__H)
#define XV_OLGX__H

#include <xview/pkg.h>
#include <xview/cms.h>
#include <xview/font.h>
#include <xview/window.h>
#include <xview_private/draw_impl.h>
#include <xview_private/xv_.h>
#include <olgx/olgx.h>

Xv_private Cms xv_set_control_cms(Xv_Window window_public, Xv_Drawable_info *info, int cms_status);
Xv_private Graphics_info *xv_init_olgx(register Xv_Window win, int *three_d, Xv_Font text_font);

#endif
