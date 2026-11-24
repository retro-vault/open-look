/* The supplied square clock frame, with a live dial rendered at any size. */
#ifndef _DEFAULT_SOURCE
#define _DEFAULT_SOURCE
#endif
#include "clock-face.h"
#include "app-icon.h"
#include <xview/cms.h>
#include <X11/Xutil.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

static const double turn = 6.28318530717958647692;

/* Read the complete local time: neither hand jumps to the preceding hour/minute. */
void clock_hand_angles(const struct tm *time, double *hour, double *minute, double *second)
{
    *second = turn * time->tm_sec / 60.0;
    *minute = turn * (time->tm_min + time->tm_sec / 60.0) / 60.0;
    *hour = turn * (time->tm_hour % 12 + time->tm_min / 60.0 + time->tm_sec / 3600.0) / 12.0;
}

/* Free only the renderer's buffers; shared theme artwork belongs to app_icons. */
void clock_face_destroy(ClockFace *face)
{
    if (face->display) {
        if (face->face) XFreePixmap(face->display, face->face);
        if (face->buffer) XFreePixmap(face->display, face->buffer);
        if (face->gc) XFreeGC(face->display, face->gc);
    }
    memset(face, 0, sizeof *face);
}

/* Draw radial marks in the theme's original pixel coordinates, then scale. */
static void radial_line(ClockFace *face, Drawable drawable, double angle,
                        double start, double end, double thickness)
{
    int width = (int)lround(thickness * face->scale);
    XSetLineAttributes(face->display, face->gc, width < 1 ? 1 : width,
                       LineSolid, CapButt, JoinMiter);
    XDrawLine(face->display, drawable, face->gc,
        (int)lround(face->cx + sin(angle) * start * face->scale),
        (int)lround(face->cy - cos(angle) * start * face->scale),
        (int)lround(face->cx + sin(angle) * end * face->scale),
        (int)lround(face->cy - cos(angle) * end * face->scale));
}

/* Cache a scaled, hand-free face. The 40px frame starts at (12,4) in the PNG;
 * clear its white interior and redraw evenly spaced, legible hour markers. */
static int prepare_face(ClockFace *face, Xv_window window, int compact,
                         const AppIconImage *art)
{
    Display *display = (Display *)xv_get(window, XV_DISPLAY);
    Window xid = (Window)xv_get(window, XV_XID);
    XWindowAttributes wa;
    if (!XGetWindowAttributes(display, xid, &wa) || wa.width < 1 || wa.height < 1) return 0;
    Cms cms = xv_get(window, WIN_CMS);
    unsigned long background = (unsigned long)xv_get(cms, CMS_BACKGROUND_PIXEL);
    if (face->face && face->width == wa.width && face->height == wa.height &&
        face->compact == compact && face->background == background) return 1;
    clock_face_destroy(face);
    face->display = display; face->width = wa.width; face->height = wa.height;
    face->compact = compact; face->background = background;
    int side = compact ? 40 : (wa.width < wa.height ? wa.width : wa.height) - 12;
    if (side < 12) side = 12;
    int left = (wa.width - side) / 2, top = (wa.height - side) / 2;
    if (compact) top -= 8; /* Match the artwork's clearance above the label. */
    face->scale = side / 40.0;
    face->cx = left + side / 2.0; face->cy = top + side / 2.0;
    XImage *source = XGetImage(display, (Pixmap)xv_get(art->image, XV_XID),
        0, 0, 64, 64, AllPlanes, ZPixmap);
    XImage *mask = art->mask ? XGetImage(display, (Pixmap)xv_get(art->mask, XV_XID),
        0, 0, 64, 64, 1, ZPixmap) : NULL;
    XImage *scaled = XCreateImage(display, wa.visual, wa.depth, ZPixmap,
        0, NULL, wa.width, wa.height, 32, 0);
    if (!source || !scaled || (art->mask && !mask)) goto fail;
    scaled->data = calloc(wa.height, scaled->bytes_per_line);
    if (!scaled->data) goto fail;
    face->black = XGetPixel(source, 32, 4);
    unsigned long white = XGetPixel(source, 32, 10);
    for (int y = 0; y < wa.height; ++y) {
        for (int x = 0; x < wa.width; ++x) {
            int sx = (int)floor(12 + (x - left) / face->scale);
            int sy = (int)floor(4 + (y - top) / face->scale);
            unsigned long pixel = background;
            if (sx >= 0 && sx < 64 && sy >= 0 && sy < 64 &&
                (!mask || XGetPixel(mask, sx, sy))) {
                pixel = XGetPixel(source, sx, sy);
                if (sx >= 18 && sx <= 45 && sy >= 10 && sy <= 37) pixel = white;
            }
            XPutPixel(scaled, x, y, pixel);
        }
    }
    face->face = XCreatePixmap(display, xid, wa.width, wa.height, wa.depth);
    face->buffer = XCreatePixmap(display, xid, wa.width, wa.height, wa.depth);
    face->gc = XCreateGC(display, face->face, 0, NULL);
    XPutImage(display, face->face, face->gc, scaled, 0, 0, 0, 0, wa.width, wa.height);
    XSetForeground(display, face->gc, face->black);
    for (int i = 0; i < 12; ++i)
        radial_line(face, face->face, turn * i / 12.0,
            i % 3 ? 10.5 : 9.5, 12.5, i % 3 ? 1.0 : 1.5);
    XDestroyImage(source); XDestroyImage(scaled);
    if (mask) XDestroyImage(mask);
    return 1;
fail:
    if (source) XDestroyImage(source);
    if (mask) XDestroyImage(mask);
    if (scaled) XDestroyImage(scaled);
    clock_face_destroy(face);
    return 0;
}

/* Recompose each frame from the clean dial, avoiding XOR trails after exposure,
 * resize, a time change, or switching between the window and minimized icon. */
void clock_face_paint(ClockFace *face, Xv_window window, const struct tm *time,
                      int seconds, int compact)
{
    const AppIconImage *art = app_icon_image(window, "clock");
    if (!art || !prepare_face(face, window, compact, art)) return;
    double hour, minute, second;
    clock_hand_angles(time, &hour, &minute, &second);
    XSetClipMask(face->display, face->gc, None);
    XCopyArea(face->display, face->face, face->buffer, face->gc,
        0, 0, face->width, face->height, 0, 0);
    XSetForeground(face->display, face->gc, face->black);
    radial_line(face, face->buffer, hour, -0.5, 8.0, 2.5);
    radial_line(face, face->buffer, minute, -0.5, 10.5, 2.0);
    if (seconds) radial_line(face, face->buffer, second, -2.0, 12.0, 0.6);
    int hub = (int)lround(2.5 * face->scale);
    if (hub < 2) hub = 2;
    XFillArc(face->display, face->buffer, face->gc,
        (int)lround(face->cx) - hub / 2, (int)lround(face->cy) - hub / 2,
        hub, hub, 0, 360 * 64);
    /* Native ICON paints the workspace around the image's transparency mask. */
    if (compact && art->mask)
        XSetClipMask(face->display, face->gc, (Pixmap)xv_get(art->mask, XV_XID));
    XCopyArea(face->display, face->buffer, (Window)xv_get(window, XV_XID), face->gc,
        0, 0, face->width, face->height, 0, 0);
    XSetClipMask(face->display, face->gc, None);
}
