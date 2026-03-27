/*
 * Declares private XView interfaces, types, and macros for fm get.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(FM_GET__H)
#define FM_GET__H

#include <xview/pkg.h>
#include <xview/frame.h>
#include <xview_private/fm_impl.h>

Pkg_private Xv_opaque frame_get_attr(Frame frame_public, int *status, Attr_attribute attr, va_list valist);
Pkg_private Frame_menu_accelerator *frame_find_menu_acc(Frame frame_public, int keycode, unsigned int state, KeySym keysym, int remove);

#endif
