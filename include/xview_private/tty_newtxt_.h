/*
 * Declares private XView interfaces, types, and macros for tty newtxt.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(TTY_NEWTXT__H)
#define TTY_NEWTXT__H

#include <xview/pkg.h>
#include <xview_private/tty_impl.h>
#include <xview_private/draw_impl.h>
#include <X11/Xlib.h>

static GC create_GC(Display *display, Drawable drawable, int foreground, int background, int function);
static GC *get_gc_list(Xv_Drawable_info *info);
static void setup_font(Xv_opaque window, Xv_opaque pixfont);
static void setup_GC(Display *display, Xv_Drawable_info *info, GC gc, int pix_op);
static void firsttime_init(void);

Xv_private void tty_newtext(Xv_opaque window, register int xbasew, register int ybasew, int op, Xv_opaque pixfont, CHAR *string, int len);
Xv_private void tty_background(Xv_opaque window, int x, int y, int w, int h, int op);
Xv_private void tty_copyarea(Xv_opaque window, int sX, int sY, int W, int H, int dX, int dY);
Xv_private void tty_synccopyarea(Xv_opaque window);
Xv_private Tty_exposed_lines *tty_calc_exposed_lines(Xv_opaque window, XEvent *first_event, int caret_y);
void tty_clear_clip_rectangles(Xv_opaque window);

#endif
