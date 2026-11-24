/* Check real pixels as well as hand angles: fixed times, second-hand removal,
 * resize, minimized rendering, and repainting without stale hands. */
#define _DEFAULT_SOURCE
#include "../apps/clock/clock-face.h"
#include "app-icon.h"
#include <xview/frame.h>
#include <X11/Xutil.h>
#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <unistd.h>

static const double quarter_turn = 1.57079632679489661923;

static int black_at(ClockFace *face, double dx, double dy)
{
    /* Read the completed frame, independent of window-manager occlusion. */
    XImage *image = XGetImage(face->display, face->buffer,
        (int)lround(face->cx + dx * face->scale),
        (int)lround(face->cy + dy * face->scale), 1, 1, AllPlanes, ZPixmap);
    assert(image);
    int black = XGetPixel(image, 0, 0) == face->black;
    XDestroyImage(image);
    return black;
}

int main(void)
{
    struct tm time = {0};
    double hour, minute, second;
    for (int h = 0; h < 24; h += 3) {
        time.tm_hour = h;
        clock_hand_angles(&time, &hour, &minute, &second);
        assert(fabs(hour - (h % 12) / 3 * quarter_turn) < 1e-12);
        assert(minute == 0 && second == 0);
    }
    time = (struct tm){.tm_hour = 6, .tm_min = 30, .tm_sec = 30};
    clock_hand_angles(&time, &hour, &minute, &second);
    assert(fabs(hour / quarter_turn - (2.0 + 61.0 / 360.0)) < 1e-12);
    assert(fabs(minute / quarter_turn - 61.0 / 30.0) < 1e-12);
    assert(fabs(second / quarter_turn - 2.0) < 1e-12);
    xv_init(NULL);
    Frame frame = xv_create(XV_NULL, FRAME, XV_WIDTH, 160, XV_HEIGHT, 160,
        FRAME_LABEL, "Clock face test", FRAME_NO_CONFIRM, TRUE, NULL);
    Display *display = (Display *)xv_get(frame, XV_DISPLAY);
    Window xid = (Window)xv_get(frame, XV_XID);
    XSetWindowAttributes unmanaged = {.override_redirect = True};
    XChangeWindowAttributes(display, xid, CWOverrideRedirect, &unmanaged);
    xv_set(frame, XV_SHOW, TRUE, NULL);
    XWindowAttributes attributes;
    for (int i = 0; i < 200; ++i) {
        XGetWindowAttributes(display, (Window)xv_get(frame, XV_XID), &attributes);
        if (attributes.map_state == IsViewable) break;
        usleep(10000);
    }
    assert(attributes.map_state == IsViewable);
    ClockFace face = {0};
    for (int pass = 0; pass < 3; ++pass) {
        int width = pass == 0 ? 160 : pass == 1 ? 260 : 64;
        int height = pass == 0 ? 160 : pass == 1 ? 180 : 64;
        XResizeWindow(display, xid, width, height);
        XSync(display, False);
        for (int h = 3; h <= 12; h += 3) {
            time = (struct tm){.tm_hour = h};
            clock_face_paint(&face, frame, &time, 0, pass == 2);
            assert(face.width == width && face.height == height);
            assert(black_at(&face, 0, -6)); /* minute hand */
            assert(black_at(&face, 6, 0) == (h == 3));
            assert(black_at(&face, 0, 6) == (h == 6));
            assert(black_at(&face, -6, 0) == (h == 9));
        }
        time = (struct tm){.tm_hour = 3, .tm_sec = 30};
        clock_face_paint(&face, frame, &time, 1, pass == 2);
        assert(black_at(&face, 0, 6));
        clock_face_paint(&face, frame, &time, 0, pass == 2);
        assert(!black_at(&face, 0, 6));
    }
    clock_face_destroy(&face);
    xv_destroy(frame);
    puts("Clock hand positions, fractional time, seconds, repaint, resize and icon rendering passed.");
    return 0;
}
