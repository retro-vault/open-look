/*
 * Declares public XView interfaces, types, and attributes for win
 * damage.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(WIN_DAMAGE__H)
#define WIN_DAMAGE__H

#include <xview/pkg.h>
#include <xview/rect.h>
#include <xview/rectlist.h>
#include <xview/win_input.h>
#include <X11/Xresource.h>

void win_clear_damage(Xv_object window);
void win_set_damage(Xv_object window, Rectlist *rl);
Rectlist *win_get_damage(Xv_object window);
Xv_private int win_convert_to_x_rectlist(Rectlist *rl, XRectangle *xrect_array, int xrect_count);
Xv_public int win_set_clip(register Xv_object window, Rectlist *rl);
Pkg_private int win_do_expose_event(Display *display, Event *event, register XExposeEvent *e, Xv_opaque *window, short collapse_exposures);
#ifdef _XV_DEBUG
win_expose_status(Display *display, XID d);
#endif

#endif
