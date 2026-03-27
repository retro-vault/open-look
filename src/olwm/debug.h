/*
 * debug.h: declarations and shared types for the debug module.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */

#ifndef _OLWM_DEBUG_H
#define _OLWM_DEBUG_H

/* DebugEvent: Process debug event. */
void DebugEvent(XEvent *ep, char *str);
/* DebugWindow: Process debug window. */
void DebugWindow(WinGeneric *win);

#endif /* _OLWM_DEBUG_H */
