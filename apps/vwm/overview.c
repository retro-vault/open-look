/* Desktop overview drawing and direct manipulation. The back buffer belongs
 * to Vwm; dragging sends a desktop assignment, never reparents client windows. */
#include "vwm.h"
#include <string.h>
#include <stdlib.h>

/* Pane dimensions stay fixed as rows and columns are added or removed. */
static XRectangle
cell_rect(Vwm *vwm, unsigned long desktop)
{
    XRectangle cell = {
        (short)((desktop % vwm->model.columns) * VWM_CELL_WIDTH),
        (short)((desktop / vwm->model.columns) * VWM_CELL_HEIGHT),
        VWM_CELL_WIDTH, VWM_CELL_HEIGHT
    };
    return cell;
}

static int
contains(XRectangle rect, int x, int y)
{
    return x >= rect.x && y >= rect.y &&
           x < rect.x + rect.width && y < rect.y + rect.height;
}

static int
desktop_at(Vwm *vwm, int x, int y)
{
    for (unsigned long i = 0; i < vwm->model.columns * vwm->model.rows; ++i)
        if (contains(cell_rect(vwm, i), x, y))
            return (int)i;
    return -1;
}

static XRectangle
window_rect(Vwm *vwm, XRectangle cell, const VwmWindow *window)
{
    int sw = vwm->model.screen_width;
    int sh = vwm->model.screen_height;
    if (sw < 1) sw = 1;
    if (sh < 1) sh = 1;
    int width = (int)cell.width - 4;
    int height = (int)cell.height - 4;
    int w = window->width * width / sw;
    int h = window->height * height / sh;
    XRectangle rect = {
        (short)(cell.x + 2 + window->x * width / sw),
        (short)(cell.y + 2 + window->y * height / sh),
        (unsigned short)(w < 8 ? 8 : w),
        (unsigned short)(window->iconic ? 12 : h < 8 ? 8 : h)
    };
    return rect;
}

static Window
window_at(Vwm *vwm, int desktop, int x, int y)
{
    if (desktop < 0)
        return None;
    XRectangle cell = cell_rect(vwm, (unsigned long)desktop);
    for (size_t i = vwm->model.count; i; --i) {
        VwmWindow *window = &vwm->model.windows[i - 1];
        if ((window->desktop == (unsigned long)desktop ||
             window->desktop == DESKTOP_ALL) &&
            contains(window_rect(vwm, cell, window), x, y))
            return window->window;
    }
    return None;
}

static void
draw_window(Vwm *vwm, XRectangle cell, VwmWindow *window)
{
    XRectangle rect = window_rect(vwm, cell, window);
    XRectangle clip = {cell.x + 2, cell.y + 2,
        cell.width - 4, cell.height - 4};
    XSetClipRectangles(vwm->display, vwm->gc, 0, 0, &clip, 1, Unsorted);
    XSetForeground(vwm->display, vwm->gc, vwm->white);
    XFillRectangle(vwm->display, vwm->buffer, vwm->gc,
                    rect.x, rect.y, rect.width, rect.height);
    XSetForeground(vwm->display, vwm->gc, vwm->black);
    XDrawRectangle(vwm->display, vwm->buffer, vwm->gc,
                    rect.x, rect.y, rect.width, rect.height);
    /* A small title strip keeps miniatures recognizable at the compact size. */
    if (!window->iconic && rect.height >= 10) {
        XDrawLine(vwm->display, vwm->buffer, vwm->gc,
                  rect.x + 1, rect.y + 4, rect.x + rect.width - 1, rect.y + 4);
        if (rect.width >= 12)
            XDrawLine(vwm->display, vwm->buffer, vwm->gc,
                      rect.x + 3, rect.y + 2, rect.x + 5, rect.y + 2);
    }
    if (rect.height >= 22 && rect.width > 30) {
        int chars = ((int)rect.width - 6) / 7;
        if (chars > (int)strlen(window->title))
            chars = (int)strlen(window->title);
        XDrawString(vwm->display, vwm->buffer, vwm->gc,
                     rect.x + 3, rect.y + 17, window->title, chars);
    }
    if (window->desktop == DESKTOP_ALL)
        XFillRectangle(vwm->display, vwm->buffer, vwm->gc,
                        rect.x + rect.width - 5, rect.y + 3, 3, 3);
    XSetClipMask(vwm->display, vwm->gc, None);
}

void
vwm_overview_draw(Vwm *vwm)
{
    if (!vwm->paint || !vwm->gc)
        return;
    vwm->width = (int)xv_get(vwm->paint, XV_WIDTH);
    vwm->height = (int)xv_get(vwm->paint, XV_HEIGHT);
    if (vwm->width < 1 || vwm->height < 1)
        return;
    Window xid = (Window)xv_get(vwm->paint, XV_XID);
    if (!vwm->buffer || vwm->buffer_width != vwm->width ||
        vwm->buffer_height != vwm->height) {
        if (vwm->buffer)
            XFreePixmap(vwm->display, vwm->buffer);
        vwm->buffer = XCreatePixmap(vwm->display, xid,
            vwm->width, vwm->height, (unsigned int)xv_get(vwm->paint, WIN_DEPTH));
        vwm->buffer_width = vwm->width;
        vwm->buffer_height = vwm->height;
    }
    XSetForeground(vwm->display, vwm->gc, vwm->grey);
    XFillRectangle(vwm->display, vwm->buffer, vwm->gc,
                    0, 0, vwm->width, vwm->height);
    if (vwm->model.available) {
        unsigned long count = vwm->model.columns * vwm->model.rows;
        for (unsigned long i = 0; i < count; ++i) {
            XRectangle cell = cell_rect(vwm, i);
            for (size_t j = 0; j < vwm->model.count; ++j) {
                VwmWindow *window = &vwm->model.windows[j];
                if (window->desktop == i || window->desktop == DESKTOP_ALL)
                    draw_window(vwm, cell, window);
            }
        }
        /* Draw the shared boundaries last so each separator stays continuous. */
        const char dashes[] = {3, 3};
        XSetForeground(vwm->display, vwm->gc, vwm->black);
        XSetDashes(vwm->display, vwm->gc, 0, dashes, sizeof dashes);
        XSetLineAttributes(vwm->display, vwm->gc, 1, LineOnOffDash,
                           CapButt, JoinMiter);
        for (unsigned long column = 1; column < vwm->model.columns; ++column) {
            int x = (int)column * VWM_CELL_WIDTH;
            XDrawLine(vwm->display, vwm->buffer, vwm->gc,
                      x, 0, x, vwm->height - 1);
        }
        for (unsigned long row = 1; row < vwm->model.rows; ++row) {
            int y = (int)row * VWM_CELL_HEIGHT;
            XDrawLine(vwm->display, vwm->buffer, vwm->gc,
                      0, y, vwm->width - 1, y);
        }
        XSetLineAttributes(vwm->display, vwm->gc, 0, LineSolid,
                           CapButt, JoinMiter);
        /* Outline all four edges of the current cell on the grid boundaries.
         * Keep outer edges inside the drawable so none are clipped away. */
        XRectangle current = cell_rect(vwm, vwm->model.current);
        int right = current.x + current.width;
        int bottom = current.y + current.height;
        if (right >= vwm->width)
            right = vwm->width - 1;
        if (bottom >= vwm->height)
            bottom = vwm->height - 1;
        XDrawRectangle(vwm->display, vwm->buffer, vwm->gc,
                        current.x, current.y,
                        right - current.x, bottom - current.y);
    }
    XCopyArea(vwm->display, vwm->buffer, xid, vwm->gc,
                0, 0, vwm->width, vwm->height, 0, 0);
}

void
vwm_overview_event(Xv_window window, Event *event)
{
    Vwm *vwm = (Vwm *)xv_get(window, WIN_CLIENT_DATA);
    if (!vwm->model.available)
        return;
    int x = event_x(event), y = event_y(event);
    if (event_action(event) == ACTION_ADJUST && event_is_down(event)) {
        Window target = window_at(vwm, desktop_at(vwm, x, y), x, y);
        for (size_t i = 0; i < vwm->model.count; ++i) {
            VwmWindow *client = &vwm->model.windows[i];
            if (client->window == target) {
                vwm_model_request(&vwm->model, target, vwm->model.desktop_atom,
                    client->desktop == DESKTOP_ALL ?
                    vwm->model.current : DESKTOP_ALL, 2);
                break;
            }
        }
    } else if (event_action(event) == ACTION_SELECT && event_is_down(event)) {
        vwm->press_desktop = desktop_at(vwm, x, y);
        vwm->dragged = window_at(vwm, vwm->press_desktop, x, y);
        vwm->press_x = x;
        vwm->press_y = y;
        vwm_overview_draw(vwm);
    } else if (event_action(event) == ACTION_SELECT && event_is_up(event)) {
        int target = desktop_at(vwm, x, y);
        VwmModel *model = &vwm->model;
        if (vwm->dragged && target >= 0 && target != vwm->press_desktop) {
            vwm_model_request(model, vwm->dragged, model->desktop_atom,
                               (unsigned long)target, 2);
        } else if (target >= 0 && abs(x - vwm->press_x) < 5 &&
                   abs(y - vwm->press_y) < 5) {
            /* A miniature must not swallow a click intended for its desktop. */
            vwm_model_request(model, model->root, model->current_atom,
                               (unsigned long)target, CurrentTime);
            model->current = (unsigned long)target;
            vwm_overview_draw(vwm);
            if (vwm->dragged) {
                unsigned long now = event_xevent(event)->xbutton.time;
                if (vwm->last_click_window == vwm->dragged &&
                    now - vwm->last_click_time < 400)
                    vwm_model_request(model, vwm->dragged,
                                       model->activate_atom, 0, 0);
                vwm->last_click_window = vwm->dragged;
                vwm->last_click_time = now;
            }
        }
        vwm->dragged = None;
    }
}
