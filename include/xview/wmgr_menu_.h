/*
 * Declares public XView interfaces, types, and attributes for wmgr
 * menu.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(WMGR_MENU__H)
#define WMGR_MENU__H

#include <xview/pkg.h>
#include <xview/frame.h>

Xv_public void wmgr_open(Frame frame_public);
Xv_public void wmgr_close(Frame frame_public);
Xv_public void wmgr_top(Frame frame);
Xv_public void wmgr_bottom(Frame frame);
Pkg_private int wmgr_forktool(char *programname, char *otherargs, struct rect *rectnormal, struct rect *recticon, int iconic);
Pkg_private int wmgr_constructargs(char *args[], char *programname, char *otherargs, int maxargcount);

#endif
