/* Read a real X11 PRIMARY text selection for native-widget interaction tests.
 * This process owns its request window and bounds the selection wait. */
#define _POSIX_C_SOURCE 200809L
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <stdio.h>
#include <time.h>

int
main(void)
{
    Display *display = XOpenDisplay(NULL);
    if (!display)
        return 1;
    Window window = XCreateSimpleWindow(display, DefaultRootWindow(display),
                                        0, 0, 1, 1, 0, 0, 0);
    Atom property = XInternAtom(display, "_OPENLOOK_TEST_SELECTION", False);
    XConvertSelection(display, XA_PRIMARY, XA_STRING, property,
                       window, CurrentTime);
    XFlush(display);
    int result = 1;
    for (int i = 0; i < 300; ++i) {
        if (XPending(display)) {
            XEvent event;
            XNextEvent(display, &event);
            if (event.type != SelectionNotify)
                continue;
            if (event.xselection.property == None)
                break;
            Atom type;
            int format;
            unsigned long count, remaining;
            unsigned char *data = NULL;
            if (XGetWindowProperty(display, window, property, 0, 1024, True,
                XA_STRING, &type, &format, &count, &remaining, &data) == Success &&
                type == XA_STRING && format == 8 && !remaining) {
                fwrite(data, 1, count, stdout);
                result = 0;
            }
            if (data)
                XFree(data);
            break;
        }
        struct timespec delay = {0, 10000000};
        nanosleep(&delay, NULL);
    }
    XDestroyWindow(display, window);
    XCloseDisplay(display);
    return result;
}
