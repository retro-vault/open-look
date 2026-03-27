/*
 * Declares public XView interfaces, types, and attributes for wmgr
 * decor.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(WMGR_DECOR__H)
#define WMGR_DECOR__H

#include <xview/pkg.h>
#include <xview/frame.h>
#include <xview_private/wmgr_decor.h>

Xv_private int wmgr_add_decor(Frame frame_public, Atom *decor_list, int num_of_decor);
Xv_private int wmgr_delete_decor(Frame frame_public, Atom *decor_list, int num_of_decor);
Xv_private int wmgr_set_win_attr(Frame frame_public, WM_Win_Type *win_attr);
Xv_private void wmgr_set_rescale_state(Frame frame_public, int state);

#endif
