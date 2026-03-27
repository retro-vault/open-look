/*
 * error.h: declarations and shared types for the error module.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */

#ifndef _OLWM_ERROR_H
#define _OLWM_ERROR_H

#include <X11/Xlib.h>

/* ErrorSensitive: Process error sensitive. */
void ErrorSensitive(char *s);
/* ErrorInsensitive: Process error insensitive. */
void ErrorInsensitive(Display *dpy);
/* ErrorHandler: Process error handler. */
int ErrorHandler(Display *dpy, XErrorEvent *event);
/* ErrorGeneral: Process error general. */
void ErrorGeneral(char *txt);
/* ErrorWarning: Process error warning. */
void ErrorWarning(char *txt);

#endif /* _OLWM_ERROR_H */
