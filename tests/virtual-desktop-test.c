/* Exercise olwm desktop lifecycle against a nested X server. The test owns its
 * windows and WM child; the X server stays alive through shutdown and restart. */
#define _DEFAULT_SOURCE
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <assert.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#include "../src/olwm/desktop-protocol.h"

static Display *display;
static Window root;
static pid_t manager;

static Atom
atom(const char *name)
{
    return XInternAtom(display, name, False);
}

static void
settle(void)
{
    XSync(display, False);
    usleep(100000);
}

static unsigned long
value(Window window, const char *name)
{
    unsigned char *data = NULL;
    unsigned long count, remaining, result = 0;
    int format;
    Atom type;
    assert(XGetWindowProperty(display, window, atom(name), 0, 2, False,
        AnyPropertyType, &type, &format, &count, &remaining, &data) == Success);
    if (format == 32 && count)
        result = ((unsigned long *)data)[0] & DESKTOP_ALL;
    if (data)
        XFree(data);
    return result;
}

static Window
parent(Window window)
{
    Window top, result, *children = NULL;
    unsigned int count;
    assert(XQueryTree(display, window, &top, &result, &children, &count));
    if (children)
        XFree(children);
    return result;
}

static XWindowAttributes
geometry(Window window)
{
    XWindowAttributes attributes;
    assert(XGetWindowAttributes(display, window, &attributes));
    return attributes;
}

static void
request(Window target, const char *name, unsigned long first,
        unsigned long second)
{
    XEvent event = {0};
    event.xclient.type = ClientMessage;
    event.xclient.window = target;
    event.xclient.message_type = atom(name);
    event.xclient.format = 32;
    event.xclient.data.l[0] = (long)first;
    event.xclient.data.l[1] = (long)second;
    assert(XSendEvent(display, root, False,
        SubstructureRedirectMask | SubstructureNotifyMask, &event));
    settle();
}

static void
switch_to(unsigned long desktop)
{
    request(root, "_NET_CURRENT_DESKTOP", desktop, CurrentTime);
    assert(value(root, "_NET_CURRENT_DESKTOP") == desktop);
}

static Window
create_window(const char *name, Window owner, int x, Bool sticky)
{
    Window window = XCreateSimpleWindow(display, root, x, 120,
                                        220, 150, 0, 0, 0xffffff);
    XSizeHints size = {.flags = USPosition | USSize,
        .x = x, .y = 120, .width = 220, .height = 150};
    XWMHints hints = {.flags = InputHint, .input = True};
    XSetWMNormalHints(display, window, &size);
    XSetWMHints(display, window, &hints);
    XStoreName(display, window, name);
    if (sticky) {
        unsigned long desktop = DESKTOP_ALL;
        XChangeProperty(display, window, atom("_NET_WM_DESKTOP"), XA_CARDINAL,
                         32, PropModeReplace, (unsigned char *)&desktop, 1);
    }
    if (owner)
        XSetTransientForHint(display, window, owner);
    XMapWindow(display, window);
    settle();
    assert(parent(window) != root);
    return window;
}

static void
start_manager(void)
{
    manager = fork();
    assert(manager >= 0);
    if (!manager) {
        char path[4096];
        snprintf(path, sizeof path, "%s/bin/olwm", getenv("OPENWINHOME"));
        execl(path, path, (char *)NULL);
        _exit(127);
    }
    for (int i = 0; i < 100; ++i) {
        if (value(root, DESKTOP_VERSION) == 1) {
            settle();
            return;
        }
        usleep(20000);
    }
    assert(!"olwm failed to start");
}

static void
stop_manager(void)
{
    assert(kill(manager, SIGTERM) == 0);
    for (int i = 0; i < 100; ++i) {
        int status;
        if (waitpid(manager, &status, WNOHANG) == manager) {
            manager = 0;
            assert(WIFEXITED(status) && WEXITSTATUS(status) == 0);
            settle();
            return;
        }
        usleep(20000);
    }
    kill(manager, SIGKILL);
    assert(!"olwm failed to exit with the server alive");
}

/* Resize Xephyr through its host, exercising the existing RandR path. */
static void
resize_server(void)
{
    Display *host = XOpenDisplay(":99");
    assert(host);
    Window top, p, *children;
    unsigned int count;
    assert(XQueryTree(host, DefaultRootWindow(host), &top, &p, &children, &count));
    assert(count == 1);
    XResizeWindow(host, children[0], 640, 480);
    XFree(children);
    XSync(host, False);
    for (int i = 0; i < 100 && geometry(root).width != 640; ++i)
        usleep(20000);
    assert(geometry(root).width == 640);
    settle();
    XCloseDisplay(host);
}

int
main(void)
{
    display = XOpenDisplay(NULL);
    assert(display);
    root = DefaultRootWindow(display);
    start_manager();
    assert(value(root, "_NET_NUMBER_OF_DESKTOPS") == 1);
    Window a = create_window("Desktop A", None, 60, False);
    Window b = create_window("Desktop B", None, 800, False);
    Window pinned = create_window("Initially sticky", None, 400, True);
    assert(value(pinned, "_NET_WM_DESKTOP") == DESKTOP_ALL);
    XWindowAttributes before = geometry(parent(b));
    request(root, DESKTOP_GRID, 3, 2);
    assert(value(root, "_NET_NUMBER_OF_DESKTOPS") == 6);
    request(b, "_NET_WM_DESKTOP", 1, 2);
    assert(geometry(a).map_state == IsViewable);
    assert(geometry(b).map_state == IsUnviewable);
    assert(value(b, "WM_STATE") == NormalState);
    switch_to(1);
    assert(geometry(a).map_state == IsUnviewable);
    assert(geometry(b).map_state == IsViewable);
    assert(geometry(pinned).map_state == IsViewable);
    assert(geometry(parent(b)).x == before.x);
    Window focus;
    int revert;
    XGetInputFocus(display, &focus, &revert);
    assert(focus != a);
    Window dialog = create_window("Dependent dialog", b, 150, False);
    assert(value(dialog, "_NET_WM_DESKTOP") == 1);
    request(b, "_NET_WM_DESKTOP", 2, 2);
    assert(geometry(dialog).map_state == IsUnviewable);
    assert(value(dialog, "_NET_WM_DESKTOP") == 2);
    assert(value(pinned, "_NET_WM_DESKTOP") == DESKTOP_ALL);
    assert(geometry(pinned).map_state == IsViewable);
    switch_to(2);
    assert(geometry(dialog).map_state == IsViewable);
    request(b, "_NET_WM_DESKTOP", DESKTOP_ALL, 2);
    switch_to(0);
    assert(geometry(b).map_state == IsViewable);
    assert(geometry(dialog).map_state == IsViewable);
    request(dialog, "_NET_WM_DESKTOP", 2, 2);
    assert(value(b, "_NET_WM_DESKTOP") == 2);
    assert(geometry(b).map_state == IsUnviewable);
    Window background = create_window("Background dialog", b, 180, False);
    assert(value(background, "_NET_WM_DESKTOP") == 2);
    assert(geometry(background).map_state == IsUnviewable);
    XGetInputFocus(display, &focus, &revert);
    assert(focus != background);
    puts("Switching, positions, dependent dialogs and sticky groups passed.");

    XIconifyWindow(display, b, DefaultScreen(display));
    settle();
    assert(value(b, "WM_STATE") == IconicState);
    switch_to(2);
    assert(value(b, "WM_STATE") == IconicState);
    request(b, DESKTOP_ACTIVATE, 0, 0);
    assert(geometry(b).map_state == IsViewable);
    assert(value(b, "WM_STATE") == NormalState);
    switch_to(0);
    resize_server();
    assert(geometry(b).map_state == IsUnviewable);
    assert(value(b, "_NET_WM_DESKTOP") == 2);
    switch_to(2);
    assert(geometry(parent(b)).x >= 0 && geometry(parent(b)).x < 640);
    request(root, DESKTOP_GRID, 0, 2);
    request(root, DESKTOP_GRID, 5, 2);
    request(b, "_NET_WM_DESKTOP", 99, 2);
    assert(value(root, "_NET_NUMBER_OF_DESKTOPS") == 6);
    assert(value(b, "_NET_WM_DESKTOP") == 2);
    puts("Minimize/restore, hidden-desktop resize and invalid requests passed.");

    stop_manager();
    assert(parent(a) == root && parent(b) == root);
    assert(geometry(a).map_state == IsViewable);
    assert(geometry(b).map_state == IsViewable);
    start_manager();
    assert(value(root, "_NET_CURRENT_DESKTOP") == 2);
    assert(value(pinned, "_NET_WM_DESKTOP") == DESKTOP_ALL);
    assert(geometry(pinned).map_state == IsViewable);
    assert(geometry(a).map_state == IsUnviewable);
    assert(geometry(b).map_state == IsViewable);
    assert(value(dialog, "_NET_WM_DESKTOP") == 2);
    request(b, "_NET_WM_DESKTOP", 3, 2);
    switch_to(3);
    request(root, DESKTOP_GRID, 4, 3);
    assert(value(b, "_NET_WM_DESKTOP") == 4);
    assert(value(dialog, "_NET_WM_DESKTOP") == 4);
    assert(value(root, "_NET_CURRENT_DESKTOP") == 4);
    assert(value(pinned, "_NET_WM_DESKTOP") == DESKTOP_ALL);
    request(root, DESKTOP_GRID, 2, 2);
    assert(value(b, "_NET_WM_DESKTOP") == 2);
    assert(value(root, "_NET_CURRENT_DESKTOP") == 2);
    request(b, "_NET_WM_DESKTOP", 3, 2);
    switch_to(3);
    request(root, DESKTOP_GRID, 1, 2);
    assert(value(b, "_NET_WM_DESKTOP") == 1);
    assert(value(dialog, "_NET_WM_DESKTOP") == 1);
    assert(value(root, "_NET_CURRENT_DESKTOP") == 1);
    request(root, DESKTOP_GRID, 1, 1);
    assert(geometry(a).map_state == IsViewable);
    assert(geometry(b).map_state == IsViewable);
    assert(value(b, "_NET_WM_DESKTOP") == 0);
    XWithdrawWindow(display, a, DefaultScreen(display));
    settle();
    XMapWindow(display, a);
    settle();
    assert(geometry(a).map_state == IsViewable);
    XDestroyWindow(display, dialog);
    XDestroyWindow(display, background);
    XDestroyWindow(display, b);
    XDestroyWindow(display, a);
    XDestroyWindow(display, pinned);
    settle();
    stop_manager();
    XCloseDisplay(display);
    puts("Shutdown/restart, grid reduction and withdrawal/remapping passed.");
    return 0;
}
