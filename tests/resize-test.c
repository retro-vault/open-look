/* Exercise real Xephyr host resizing and root-coordinate mouse dragging. */
#include <assert.h>
#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <xview/xview.h>
#include <xview/frame.h>
#include <xview/notify.h>

static Display *dpy, *host, *toolkit;
static Frame toolkit_frame;
extern Notify_value xv_input_pending(Display *, int);
static Window root, xephyr;
static void settle(void) { XSync(dpy, False); usleep(250000); }
static Window parent(Window win) {
    Window r, p, *children;
    unsigned int n;
    assert(XQueryTree(dpy, win, &r, &p, &children, &n));
    if (children) XFree(children);
    return p;
}
static XWindowAttributes geometry(Window win) {
    XWindowAttributes a;
    assert(XGetWindowAttributes(dpy, win, &a));
    return a;
}
static void resize(int width, int height) {
    XResizeWindow(host, xephyr, width, height);
    XSync(host, False);
    for (int i = 0; i < 100; ++i) {
        XWindowAttributes a = geometry(root);
        if (a.width == width && a.height == height) {
            settle();
            while (XPending(toolkit)) xv_input_pending(toolkit, ConnectionNumber(toolkit));
            Rect *rect = (Rect *)xv_get(toolkit_frame, WIN_SCREEN_RECT);
            assert(DisplayWidth(toolkit, 0) == width && DisplayHeight(toolkit, 0) == height);
            assert(rect->r_width == width && rect->r_height == height);
            return;
        }
        usleep(20000);
    }
    fprintf(stderr, "Xephyr did not resize to %dx%d\n", width, height);
    exit(1);
}
int main(int argc, char **argv) {
    xv_init(XV_INIT_ARGC_PTR_ARGV, &argc, argv, NULL);
    toolkit_frame = xv_create(NULL, FRAME, XV_SHOW, FALSE, NULL);
    assert(toolkit_frame);
    toolkit = (Display *)xv_get(toolkit_frame, XV_DISPLAY);
    dpy = XOpenDisplay(NULL);
    host = XOpenDisplay(":99");
    assert(dpy && host);
    root = DefaultRootWindow(dpy);
    Window r, p, *children;
    unsigned int n;
    assert(XQueryTree(host, DefaultRootWindow(host), &r, &p, &children, &n));
    assert(n == 1);
    xephyr = children[0];
    XFree(children);
    Window win = XCreateSimpleWindow(dpy, root, 900, 500, 240, 160, 0, 0, 0xffffff);
    XSizeHints hints = {.flags = USPosition | USSize, .x = 900, .y = 500,
                        .width = 240, .height = 160};
    XSetWMNormalHints(dpy, win, &hints);
    XStoreName(dpy, win, "Resize test");
    XMapWindow(dpy, win);
    settle();
    Window frame = parent(win);
    assert(frame != root);
    const int sizes[][2] = {{640,480}, {1600,900}, {800,600}, {80,60}, {1280,800}};
    for (unsigned int i = 0; i < sizeof(sizes)/sizeof(sizes[0]); ++i) {
        resize(sizes[i][0], sizes[i][1]);
        XWindowAttributes a = geometry(frame);
        fprintf(stderr, "size %dx%d frame %d,%d\n", sizes[i][0], sizes[i][1], a.x, a.y);
        assert(a.x >= 0 && a.x < sizes[i][0]);
        assert(a.y >= 0 && a.y < sizes[i][1]);
        XWarpPointer(dpy, None, root, 0, 0, 0, 0, sizes[i][0]-10, sizes[i][1]-10);
        settle();
        Window child;
        int rx, ry, wx, wy;
        unsigned int mask;
        assert(XQueryPointer(dpy, root, &r, &child, &rx, &ry, &wx, &wy, &mask));
        assert(rx == sizes[i][0]-10 && ry == sizes[i][1]-10);
    }
    /* XTest provides real input; olwm deliberately ignores synthetic device events. */
    void *xtst = dlopen("libXtst.so.6", RTLD_NOW);
    assert(xtst);
    typedef int (*ButtonFn)(Display *, unsigned int, Bool, unsigned long);
    ButtonFn button = (ButtonFn)dlsym(xtst, "XTestFakeButtonEvent");
    assert(button);
    typedef int (*GrabFn)(Display *, Bool);
    GrabFn grab_control = (GrabFn)dlsym(xtst, "XTestGrabControl");
    assert(grab_control);
    grab_control(dpy, True);
    XWindowAttributes before = geometry(frame);
    XWarpPointer(dpy, None, root, 0, 0, 0, 0, before.x+100, before.y+20);
    settle();
    button(dpy, 1, True, 0);
    settle();
    XWarpPointer(dpy, None, root, 0, 0, 0, 0, 1100, 650);
    settle();
    XWarpPointer(dpy, None, root, 0, 0, 0, 0, 1120, 670);
    settle();
    button(dpy, 1, False, 0);
    settle();
    XWindowAttributes after = geometry(frame);
    fprintf(stderr, "drag frame %d,%d -> %d,%d\n", before.x, before.y, after.x, after.y);
    assert(after.x == 1020 && after.y == 650);
    xv_destroy(toolkit_frame);
    XDestroyWindow(dpy, win);
    XCloseDisplay(dpy);
    XCloseDisplay(host);
    puts("Repeated resize, tiny-screen recovery, XView bounds, pointer coordinates, and dragging passed.");
    return 0;
}
