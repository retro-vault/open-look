/*
 * Declares private XView interfaces, types, and macros for window cms.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(WINDOW_CMS__H)
#define WINDOW_CMS__H

#include <xview/pkg.h>
#include <xview/cms.h>
#include <xview/window.h>
#include <X11/Xresource.h>

Xv_private void window_set_cms_name(Xv_Window win_public, char *new_name);
Xv_private void window_set_cms_data(Xv_Window win_public, Xv_cmsdata *cms_data);
Pkg_private void window_set_cms(Xv_Window win_public, Cms cms, int cms_bg, int cms_fg);
Pkg_private void window_set_cmap_property(Xv_Window win_public);


#endif
