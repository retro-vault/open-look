/* Send the normal window-manager close protocol to a smoke-test browser. */
#include <X11/Xlib.h>
#include <stdlib.h>

int
main(int argc, char **argv)
{
    if (argc != 2) return EXIT_FAILURE;
    Display *display = XOpenDisplay(NULL);
    if (!display) return EXIT_FAILURE;
    XEvent event = {0};
    event.xclient.type = ClientMessage;
    event.xclient.display = display;
    event.xclient.window = strtoul(argv[1], NULL, 0);
    event.xclient.message_type = XInternAtom(display, "WM_PROTOCOLS", False);
    event.xclient.format = 32;
    event.xclient.data.l[0] = XInternAtom(display, "WM_DELETE_WINDOW", False);
    event.xclient.data.l[1] = CurrentTime;
    int sent = XSendEvent(display, event.xclient.window, False, NoEventMask,
                         &event);
    XSync(display, False);
    XCloseDisplay(display);
    return sent ? EXIT_SUCCESS : EXIT_FAILURE;
}
