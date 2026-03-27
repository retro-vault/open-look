/*
 * Declares public XView interfaces, types, and attributes for fm set.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(FM_STE__H)
#define FM_STE__H

#include <xview/pkg.h>
#include <xview_private/fm_impl.h>

Pkg_private Xv_opaque frame_set_avlist(Frame frame_public, Attr_attribute avlist[]);
Pkg_private void frame_update_compose_led(Frame_class_info *frame, int state);
Pkg_private int frame_set_menu_acc(Frame frame_public, int keycode, unsigned int state, KeySym keysym, CHAR *keystr, void (*notify_proc)(), Xv_opaque data);

#endif
