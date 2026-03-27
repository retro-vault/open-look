/*
 * dsdm.h: declarations and shared types for the dsdm module.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */

#ifndef _OLWM_DSDM_H
#define _OLWM_DSDM_H

/* constants */

#define DND_VERSION		0
#define DND_INTEREST_RECT	0
#define DND_INTEREST_WINDOW	1
#define DND_SITE_DEFAULT	(1<<2)
#define DND_SITE_FORWARD	(1<<3)

/* public functions */

/* DragDropStartDSDM: Start drag drop dsdm. */
void DragDropStartDSDM(Display *dpy);
/* DragDropStopDSDM: Stop drag drop dsdm. */
void DragDropStopDSDM(Display *dpy);
/* DragDropInit: Initialize drag drop. */
void DragDropInit(void);

#endif /* _OLWM_DSDM_H */
