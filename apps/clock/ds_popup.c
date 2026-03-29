/*
 * Implements the ds_popup routines used by the clock application.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */

#include <X11/Xlib.h>
#include <xview/xview.h>
#include <xview/rect.h>

#include "ds_popup.h"

static int
clamp_int(int value, int min_value, int max_value)
{
    if (value < min_value) {
        return min_value;
    }
    if (value > max_value) {
        return max_value;
    }
    return value;
}

void
ds_position_popup(Frame baseframe, Frame popup, int location_op)
{
    Rect base_rect = *(Rect *)xv_get(baseframe, WIN_RECT);
    Rect popup_rect = *(Rect *)xv_get(popup, WIN_RECT);
    Display *display = (Display *)xv_get(baseframe, XV_DISPLAY);
    int screen_number = DefaultScreen(display);
    int screen_width = DisplayWidth(display, screen_number);
    int screen_height = DisplayHeight(display, screen_number);
    int x = base_rect.r_left;
    int y = base_rect.r_top;
    int gap = 1;

    switch (location_op) {
    case DS_POPUP_RIGHT:
        x = base_rect.r_left + base_rect.r_width + gap;
        y = base_rect.r_top;
        break;
    case DS_POPUP_LEFT:
        x = base_rect.r_left - popup_rect.r_width - gap;
        y = base_rect.r_top;
        break;
    case DS_POPUP_ABOVE:
        x = base_rect.r_left;
        y = base_rect.r_top - popup_rect.r_height - gap;
        break;
    case DS_POPUP_BELOW:
        x = base_rect.r_left;
        y = base_rect.r_top + base_rect.r_height + gap;
        break;
    case DS_POPUP_AOB:
        x = base_rect.r_left;
        y = base_rect.r_top + base_rect.r_height + gap;
        if (y + popup_rect.r_height > screen_height) {
            y = base_rect.r_top - popup_rect.r_height - gap;
        }
        break;
    case DS_POPUP_LOR:
    default:
        x = base_rect.r_left + base_rect.r_width + gap;
        y = base_rect.r_top;
        if (x + popup_rect.r_width > screen_width) {
            x = base_rect.r_left - popup_rect.r_width - gap;
        }
        break;
    }

    x = clamp_int(x, 0, (screen_width > popup_rect.r_width) ?
        (screen_width - popup_rect.r_width) : 0);
    y = clamp_int(y, 0, (screen_height > popup_rect.r_height) ?
        (screen_height - popup_rect.r_height) : 0);

    xv_set(popup, XV_X, x, XV_Y, y, NULL);
}
