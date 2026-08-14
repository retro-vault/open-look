/*
 * fallback_allplanes.c: implementation of the fallback_allplanes
 * module.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */

#include <X11/Xlib.h>

Bool XAllPlanesQueryExtension(Display *dpy, int *event_base, int *error_base)
{
    (void)dpy;
    (void)event_base;
    (void)error_base;
    return False;
}

void XAllPlanesFillRectangles(Display *dpy, Drawable d,
                              XRectangle *rects, int n)
{
    (void)dpy;
    (void)d;
    (void)rects;
    (void)n;
}

void XAllPlanesDrawSegments(Display *dpy, Drawable d,
                            XSegment *segs, int n)
{
    (void)dpy;
    (void)d;
    (void)segs;
    (void)n;
}
