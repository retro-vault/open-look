#ident	"@(#)defaults.h	26.16	93/06/28 SMI"

/*
 *      (c) Copyright 1989 Sun Microsystems, Inc.
 */

/*
 *      Sun design patents pending in the U.S. and foreign countries. See
 *      LEGAL_NOTICE file for terms of the license.
 */

#ifndef _OLWM_DEFAULTS_H
#define _OLWM_DEFAULTS_H

XrmDatabase GetUserDefaults(Display *dpy);
XrmDatabase GetAppDefaults(void);
void GetDefaults(Display *dpy, XrmDatabase commandlineDB);

#endif /* _OLWM_DEFAULTS_H */
