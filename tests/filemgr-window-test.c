/* Verify a painted File Manager window and close it with WM_DELETE_WINDOW. */
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static Window find_filemgr(Display *display, Window parent)
{
    Window root, actual_parent, *children = NULL;
    unsigned int count = 0;
    XClassHint hint = {0};
    char *name = NULL;
    Window found = None;

    if (XGetClassHint(display, parent, &hint)) {
        if (hint.res_name && strcmp(hint.res_name, "filemgr") == 0 &&
            XFetchName(display, parent, &name) && name &&
            strstr(name, "Created Folder"))
            found = parent;
        if (name) XFree(name);
        if (hint.res_name) XFree(hint.res_name);
        if (hint.res_class) XFree(hint.res_class);
    }
    if (found != None) return found;
    if (!XQueryTree(display, parent, &root, &actual_parent, &children, &count))
        return None;
    for (unsigned int i = 0; i < count && found == None; ++i)
        found = find_filemgr(display, children[i]);
    if (children) XFree(children);
    return found;
}

int main(void)
{
    Display *display = XOpenDisplay(NULL);
    Window window = None;
    XWindowAttributes attrs;
    XEvent event = {0};
    if (!display) return EXIT_FAILURE;
    for (int i = 0; i < 100; ++i) {
        window = find_filemgr(display, DefaultRootWindow(display));
        if (window != None && XGetWindowAttributes(display, window, &attrs) &&
            attrs.map_state == IsViewable)
            break;
        usleep(100000);
    }
    if (window == None) {
        fputs("No File Manager window appeared\n", stderr);
        XCloseDisplay(display);
        return EXIT_FAILURE;
    }
    /* Give the client time to handle its initial expose events. */
    usleep(500000);
    if (system("xwd -root -silent -out build/docker/filemgr-smoke.xwd") != 0)
        return EXIT_FAILURE;
    event.xclient.type = ClientMessage;
    event.xclient.display = display;
    event.xclient.window = window;
    event.xclient.message_type = XInternAtom(display, "WM_PROTOCOLS", False);
    event.xclient.format = 32;
    event.xclient.data.l[0] = (long) XInternAtom(display, "WM_DELETE_WINDOW", False);
    event.xclient.data.l[1] = CurrentTime;
    XSendEvent(display, window, False, NoEventMask, &event);
    XSync(display, False);
    XCloseDisplay(display);
    return EXIT_SUCCESS;
}
