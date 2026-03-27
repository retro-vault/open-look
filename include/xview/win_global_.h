/*
 * Declares public XView interfaces, types, and attributes for win
 * global.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(WIN_GLOBAL__H)
#define WIN_GLOBAL__H

#include <xview/pkg.h>
#include <xview/win_input.h>

void win_lockdata(Xv_object window);
void win_unlockdata(Xv_object window);
Xv_private int xv_win_grab(Xv_object window, Inputmask *im, Xv_object cursor_window, Xv_object cursor,  int grab_pointer,  int grab_kbd, int grab_server, int grap_pointer_pointer_async, int grab_pointer_keyboard_async, int grap_kbd_pointer_async, int grab_kbd_keyboard_async, int owner_events, int *status);
Xv_private void xv_win_ungrab(Xv_object window, int ungrab_pointer, int ungrab_kbd, int ungrab_server);
int win_grabio(Xv_object window);
int win_xgrabio_sync(Xv_object window, Inputmask *im, Xv_object cursor_window, Xv_object cursor);
int win_xgrabio_async(Xv_object window, Inputmask *im, Xv_object cursor_window, Xv_object cursor);
void win_set_grabio_params(Xv_object window, Inputmask *im, Xv_opaque cursor);
void win_releaseio(Xv_object window);
void win_private_gc(Xv_object window, int create_private_gc);
 
#endif
