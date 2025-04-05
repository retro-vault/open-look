#ident  "@(#)error.h	1.4    93/06/28 SMI"
 
/*
 *      (c) Copyright 1989 Sun Microsystems, Inc.
 */

/*
 *      Sun design patents pending in the U.S. and foreign countries. See
 *      LEGAL_NOTICE file for terms of the license.
 */

#ifndef _OLWM_ERROR_H
#define _OLWM_ERROR_H

#include <X11/Xlib.h>

void ErrorSensitive(char *s);
void ErrorInsensitive(Display *dpy);
int ErrorHandler(Display *dpy, XErrorEvent *event);
void ErrorGeneral(char *txt);
void ErrorWarning(char *txt);

#endif /* _OLWM_ERROR_H */
