/*
 * environ.h: declarations and shared types for the environ module.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */

#ifndef _OLWM_ENVIRON_H
#define _OLWM_ENVIRON_H

/* MakeEnviron: Create environ. */
char **MakeEnviron(Display *dpy, int screen);

#endif /* _OLWM_ENVIRON_H */
