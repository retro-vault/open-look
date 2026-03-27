/*
 * Declares public XView interfaces, types, and attributes for fm
 * display.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(FM_DISPLAY__H)
#define FM_DISPLAY__H

#include <xview/pkg.h>
#include <xview_private/fm_impl.h>


Pkg_private void frame_display_label(register Frame_class_info *frame);
Pkg_private void frame_display_footer(Frame frame_public, int clear_first);
Pkg_private void frame_update_status_win_color(Frame frame_public, Xv_Window status_window, Cms new_frame_cms, unsigned long new_frame_fg, short new_frame_fg_set, int *repaint_needed);
Pkg_private void frame_display_busy(register Frame_class_info *frame, int status);
Xv_private void frame_kbd_use(Frame frame_public, Xv_Window sw, Xv_Window pw);
Xv_private void frame_kbd_done(Frame frame_public, Xv_Window sw);
Pkg_private void frame_set_color(Frame_class_info *frame, XColor *fg, XColor *bg);
#ifdef OW_I18N
Pkg_private void frame_display_IMstatus(Frame frame_public, int clear_first);
#endif 

#endif
