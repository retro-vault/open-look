/* Query and control olwm's desktop model. A dedicated X connection isolates
 * expected BadWindow races from XView's own error handling and event queue. */
#include "vwm.h"
#include <X11/Xatom.h>
#include <X11/Xutil.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/* One query connection exists for the lifetime of this application. */
static Display *query_display;
static XErrorHandler previous_error_handler;

static int
query_error(Display *display, XErrorEvent *error)
{
    if (display == query_display && error->error_code == BadWindow)
        return 0;
    return previous_error_handler(display, error);
}

/* Caller frees the returned Xlib buffer. Bound malformed lists to 4096 XIDs. */
static unsigned long *
read_property(VwmModel *model, Window window, Atom property, Atom wanted,
              unsigned long *length)
{
    Atom type;
    int format;
    unsigned long remaining;
    unsigned char *data = NULL;
    *length = 0;
    int result = XGetWindowProperty(model->display, window, property, 0,
                                   4096, False, wanted, &type, &format,
                                   length, &remaining, &data);
    if (result != Success || type != wanted || format != 32 || remaining) {
        if (data)
            XFree(data);
        *length = 0;
        return NULL;
    }
    /* Xlib expands format-32 values into native longs, possibly sign-extending
     * the all-desktops sentinel on 64-bit hosts. Keep the wire value. */
    for (unsigned long i = 0; i < *length; ++i)
        ((unsigned long *)data)[i] &= DESKTOP_ALL;
    return (unsigned long *)data;
}

static unsigned long
read_cardinal(VwmModel *model, Window window, Atom property,
              unsigned long fallback)
{
    unsigned long length;
    unsigned long *data = read_property(model, window, property,
                                        XA_CARDINAL, &length);
    unsigned long value = length == 1 ? data[0] : fallback;
    if (data)
        XFree(data);
    return value;
}

int
vwm_model_open(VwmModel *model, Xv_window owner)
{
    memset(model, 0, sizeof *model);
    Display *toolkit = (Display *)xv_get(owner, XV_DISPLAY);
    model->display = XOpenDisplay(DisplayString(toolkit));
    if (!model->display)
        return 0;
    query_display = model->display;
    previous_error_handler = XSetErrorHandler(query_error);
    model->root = (Window)xv_get(xv_get(owner, XV_ROOT), XV_XID);
    model->own_window = (Window)xv_get(owner, XV_XID);
#define INTERN(field, name) \
    model->field = XInternAtom(model->display, name, False)
    INTERN(grid_atom, DESKTOP_GRID);
    INTERN(version_atom, DESKTOP_VERSION);
    INTERN(current_atom, "_NET_CURRENT_DESKTOP");
    INTERN(desktop_atom, "_NET_WM_DESKTOP");
    INTERN(clients_atom, "_NET_CLIENT_LIST");
    INTERN(activate_atom, DESKTOP_ACTIVATE);
    INTERN(state_atom, "WM_STATE");
#undef INTERN
    model->columns = model->rows = 1;
    return 1;
}

static int
read_window(VwmModel *model, Window window, VwmWindow *out)
{
    Window owner = None;
    if (XGetTransientForHint(model->display, window, &owner) &&
        owner == model->own_window)
        return 0;
    XWMHints *hints = XGetWMHints(model->display, window);
    int own_dialog = hints && (hints->flags & WindowGroupHint) &&
                     hints->window_group == model->own_window;
    if (hints)
        XFree(hints);
    if (own_dialog)
        return 0;
    XWindowAttributes attributes;
    Window root, parent, *children = NULL;
    unsigned int child_count;
    if (!XQueryTree(model->display, window, &root, &parent,
                    &children, &child_count))
        return 0;
    if (children)
        XFree(children);
    if (parent == model->root ||
        !XGetWindowAttributes(model->display, parent, &attributes))
        return 0;
    out->window = window;
    out->desktop = read_cardinal(model, window, model->desktop_atom, 0);
    out->x = attributes.x;
    out->y = attributes.y;
    out->width = attributes.width;
    out->height = attributes.height;
    unsigned long length;
    unsigned long *state = read_property(model, window, model->state_atom,
                                         model->state_atom, &length);
    out->iconic = length && state[0] == IconicState;
    if (state)
        XFree(state);
    char *title = NULL;
    if (XFetchName(model->display, window, &title) && title) {
        snprintf(out->title, sizeof out->title, "%s", title);
        XFree(title);
    } else {
        snprintf(out->title, sizeof out->title, "Untitled");
    }
    return 1;
}

int
vwm_model_refresh(VwmModel *model)
{
    unsigned long length;
    model->available = read_cardinal(model, model->root,
                                     model->version_atom, 0) == 1;
    if (!model->available)
        return 0;
    unsigned long *grid = read_property(model, model->root, model->grid_atom,
                                        XA_CARDINAL, &length);
    if (length == 2 && grid[0] && grid[0] <= DESKTOP_MAX_AXIS &&
        grid[1] && grid[1] <= DESKTOP_MAX_AXIS) {
        model->columns = grid[0];
        model->rows = grid[1];
    }
    if (grid)
        XFree(grid);
    model->current = read_cardinal(model, model->root, model->current_atom, 0);
    XWindowAttributes root;
    if (XGetWindowAttributes(model->display, model->root, &root)) {
        model->screen_width = root.width;
        model->screen_height = root.height;
    }
    unsigned long *list = read_property(model, model->root, model->clients_atom,
                                        XA_WINDOW, &length);
    VwmWindow *windows = calloc(length ? length : 1, sizeof *windows);
    if (!windows) {
        if (list)
            XFree(list);
        return 1;
    }
    size_t count = 0;
    for (unsigned long i = 0; i < length; ++i)
        if (list[i] != model->own_window &&
            read_window(model, list[i], &windows[count]))
            ++count;
    if (list)
        XFree(list);
    free(model->windows);
    model->windows = windows;
    model->count = count;
    return 1;
}

void
vwm_model_request(VwmModel *model, Window target, Atom action,
                  unsigned long first, unsigned long second)
{
    XEvent request = {0};
    request.xclient.type = ClientMessage;
    request.xclient.window = target;
    request.xclient.message_type = action;
    request.xclient.format = 32;
    request.xclient.data.l[0] = (long)first;
    request.xclient.data.l[1] = (long)second;
    XSendEvent(model->display, model->root, False,
               SubstructureRedirectMask | SubstructureNotifyMask, &request);
    XFlush(model->display);
}

void
vwm_model_close(VwmModel *model)
{
    free(model->windows);
    if (model->display) {
        XCloseDisplay(model->display);
        XSetErrorHandler(previous_error_handler);
    }
    query_display = NULL;
    memset(model, 0, sizeof *model);
}
