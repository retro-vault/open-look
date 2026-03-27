/*
 * Declares internal interfaces used by the XView openwin module.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(OW_PAINT__H)
#define OW_PAINT__H

#include <xview/pkg.h>
#include <xview_private/ow_impl.h>

Pkg_private void openwin_clear_damage(Xv_Window window, Rectlist *rl);
#ifdef SELECTABLE_VIEWS
void openwin_paint_borders(Openwin owin_public);
Pkg_private void openwin_paint_borderOpenwin (owin_public, Openwin_view_info *view, int on);
void openwin_hilite_view(Openwin owin_public, Openwin_view_info *view);
void openwin_lolite_viewOpenwin(owin_public, Openwin_view_info *view);
#endif 

#endif
