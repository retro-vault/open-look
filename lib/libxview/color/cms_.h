/*
 * Declares internal interfaces used by the XView color module.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(CMS__H)
#define CMS__H

#include <xview/pkg.h>
#include <X11/Xresource.h>
#include <xview/server.h>
#include <xview_private/cms_impl.h>

Pkg_private void cms_free_colors(Display *display, Cms_info *cms);
Pkg_private void cms_set_name(Cms_info *cms, char *name);
Pkg_private void cms_set_unique_name(Cms_info *cms);
Pkg_private int cms_get_colors(Cms_info *cms, unsigned long cms_index, unsigned long cms_count, Xv_Singlecolor *cms_colors, XColor *cms_x_colors, unsigned char *red, unsigned char *green, unsigned char *blue);
Pkg_private int cms_set_colors(Cms_info *cms, Xv_Singlecolor *cms_colors, XColor *cms_x_colors, unsigned long cms_index, unsigned long cms_count);
Pkg_private int cms_set_static_colors(Display *display, Cms_info *cms, XColor *xcolors, unsigned long cms_index, unsigned long cms_count);
Pkg_private int cms_alloc_static_colors(Display *display, Cms_info *cms, Xv_Colormap *cmap, XColor *xcolors, unsigned long cms_index, unsigned long cms_count);
Pkg_private int cms_set_dynamic_colors(Display *display, Cms_info *cms, XColor *xcolors, unsigned long cms_index, unsigned long cms_count);
Pkg_private XColor *cms_parse_named_colors(Cms_info *cms, char **named_colors);
Xv_private Xv_opaque cms_default_colormap(Xv_Server server, Display *display, int screen_number, XVisualInfo *vinfo);

#endif
