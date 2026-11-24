#ifndef OPENLOOK_CLOCK_FACE_H
#define OPENLOOK_CLOCK_FACE_H
#include <time.h>
#include <xview/xview.h>

typedef struct {
    Display *display;
    Pixmap face, buffer;
    GC gc;
    int width, height, compact;
    unsigned long background, black;
    double scale, cx, cy;
} ClockFace;

/* Clockwise radians from twelve, including fractional hours and minutes. */
void clock_hand_angles(const struct tm *time, double *hour, double *minute, double *second);
void clock_face_paint(ClockFace *face, Xv_window window, const struct tm *time,
                      int seconds, int compact);
void clock_face_destroy(ClockFace *face);
#endif
