/*
 * defaults.h: declarations and shared types for the defaults module.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */

#ifndef _OLWM_DEFAULTS_H
#define _OLWM_DEFAULTS_H

/* GetUserDefaults: Return user defaults. */
XrmDatabase GetUserDefaults(Display *dpy);
/* GetAppDefaults: Return app defaults. */
XrmDatabase GetAppDefaults(void);
/* GetDefaults: Return defaults. */
void GetDefaults(Display *dpy, XrmDatabase commandlineDB);

#endif /* _OLWM_DEFAULTS_H */
