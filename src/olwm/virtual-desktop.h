/* Optional desktop management hooks; clients and screens remain owned by olwm. */
#ifndef OLWM_VIRTUAL_DESKTOP_H
#define OLWM_VIRTUAL_DESKTOP_H

#include <X11/Xlib.h>
struct _client;
struct _screeninfo;

/* Initialize per-screen desktop state after InitScreens, before reparenting.
 * Borrows dpy until shutdown. Allocation failure leaves ordinary olwm active. */
void virtual_desktop_init(Display *dpy);

/* Register a fully constructed client; borrows cli until forget is called.
 * Inherits its owner's desktop, or uses its initial desktop hint. */
void virtual_desktop_add(struct _client *cli);

/* Drop all borrowed references before cli is destroyed. No X requests are
 * made to its potentially already destroyed client window. */
void virtual_desktop_forget(struct _client *cli);

/* Reapply desktop visibility after a client's normal/iconic state changes.
 * Does not modify ICCCM WM_STATE or unmap the application's own window. */
void virtual_desktop_state(struct _client *cli);

/* Return False for a client on another desktop, True for untracked clients.
 * Borrows cli; suitable for focus and keyboard-navigation filtering. */
Bool virtual_desktop_visible(const struct _client *cli);

/* Process desktop requests and shortcuts before normal event dispatch.
 * Returns True only when the event belongs to the desktop module. */
Bool virtual_desktop_event(Display *dpy, XEvent *event);

/* Publish new physical screen geometry after RandR handling. Window positions
 * remain local to each desktop, so ordinary bounds checks remain valid. */
void virtual_desktop_resize(struct _screeninfo *screen);

/* Release module memory before olwm unparents and remaps all client windows.
 * Preserve desktop properties for Restart WM; borrows dpy. */
void virtual_desktop_shutdown(Display *dpy);

#endif
