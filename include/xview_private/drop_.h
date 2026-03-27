/*
 * Declares private XView interfaces, types, and macros for drop.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(DROP__H)
#define DROP__H

#include <xview/window.h>

int xv_decode_drop(Event *ev, char* buffer, unsigned int bsize);
static int HandleNewDrop(Event *ev, XClientMessageEvent *cM, Xv_window window, char* buffer, unsigned int bsize);

#endif
