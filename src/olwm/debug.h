#ident  "@(#)debug.h	1.4    93/06/28 SMI"
 
/*
 *      (c) Copyright 1989 Sun Microsystems, Inc.
 */

/*
 *      Sun design patents pending in the U.S. and foreign countries. See
 *      LEGAL_NOTICE file for terms of the license.
 */

#ifndef _OLWM_DEBUG_H
#define _OLWM_DEBUG_H

void DebugEvent(XEvent *ep, char *str);
void DebugWindow(WinGeneric *win);

#endif /* _OLWM_DEBUG_H */
