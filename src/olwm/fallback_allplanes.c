#include <X11/Xlib.h>

Bool XAllPlanesQueryExtension(Display *dpy, int *event_base, int *error_base)
{
    return False;
}

void XAllPlanesFillRectangles(Display *dpy, Drawable d, GC gc, XRectangle *rects, int n)
{
    XFillRectangles(dpy, d, gc, rects, n); // fallback
}

void XAllPlanesDrawSegments(Display *dpy, Drawable d, GC gc, XSegment *segs, int n)
{
    XDrawSegments(dpy, d, gc, segs, n); // fallback
}
