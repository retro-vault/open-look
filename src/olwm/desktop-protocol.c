/*
 * X11 desktop properties, validated pager requests and optional key grabs.
 * EWMH desktop indices are zero-based; no synthetic input is accepted here.
 */
#include "desktop-private.h"
#include "virtual-desktop.h"
#include "client.h"
#include <X11/Xatom.h>
#include <X11/keysym.h>
#include <stdlib.h>

unsigned long
desktop_read(Display *dpy, Window win, Atom atom, unsigned long *values,
             unsigned long capacity)
{
    Atom type;
    int format;
    unsigned long count, remaining;
    unsigned char *data = NULL;
    int result = XGetWindowProperty(dpy, win, atom, 0, (long)capacity,
                                   False, XA_CARDINAL, &type, &format,
                                   &count, &remaining, &data);
    if (result != Success || type != XA_CARDINAL || format != 32 ||
        remaining || count > capacity) {
        count = 0;
    } else {
        for (unsigned long i = 0; i < count; ++i)
            values[i] = ((unsigned long *)data)[i] & DESKTOP_ALL;
    }
    if (data)
        XFree(data);
    return count;
}

static void
set_cardinals(DesktopScreen *s, Atom property, unsigned long *values, int count)
{
    XChangeProperty(s->screen->dpy, s->screen->rootid, property,
                    XA_CARDINAL, 32, PropModeReplace,
                    (unsigned char *)values, count);
}

void
desktop_publish(DesktopScreen *s)
{
    Display *dpy = s->screen->dpy;
    unsigned long count = s->columns * s->rows;
    unsigned long grid[] = {s->columns, s->rows};
    unsigned long version = 1;
    unsigned long geometry[] = {
        (unsigned long)DisplayWidth(dpy, s->screen->screen),
        (unsigned long)DisplayHeight(dpy, s->screen->screen)
    };
    unsigned long viewport[DESKTOP_MAX_COUNT * 2] = {0};
    unsigned long workarea[DESKTOP_MAX_COUNT * 4] = {0};
    Atom supported[] = {desktop_atoms.count, desktop_atoms.current,
        desktop_atoms.desktop, desktop_atoms.clients, desktop_atoms.geometry,
        desktop_atoms.viewport, desktop_atoms.workarea, desktop_atoms.roots};
    for (unsigned long i = 0; i < count; ++i) {
        workarea[i * 4 + 2] = geometry[0];
        workarea[i * 4 + 3] = geometry[1];
    }
    set_cardinals(s, desktop_atoms.grid, grid, 2);
    set_cardinals(s, desktop_atoms.version, &version, 1);
    set_cardinals(s, desktop_atoms.count, &count, 1);
    set_cardinals(s, desktop_atoms.current, &s->current, 1);
    set_cardinals(s, desktop_atoms.geometry, geometry, 2);
    set_cardinals(s, desktop_atoms.viewport, viewport, (int)count * 2);
    set_cardinals(s, desktop_atoms.workarea, workarea, (int)count * 4);
    XChangeProperty(dpy, s->screen->rootid, desktop_atoms.roots, XA_WINDOW,
                    32, PropModeReplace, NULL, 0);
    XChangeProperty(dpy, s->screen->rootid, desktop_atoms.supported, XA_ATOM,
                    32, PropModeReplace, (unsigned char *)supported,
                    sizeof supported / sizeof supported[0]);
    size_t n = 0;
    for (DesktopClient *c = desktop_clients; c; c = c->next)
        if (c->screen == s)
            ++n;
    Window *windows = calloc(n ? n : 1, sizeof *windows);
    if (!windows)
        return;
    size_t i = 0;
    for (DesktopClient *c = desktop_clients; c; c = c->next)
        if (c->screen == s)
            windows[i++] = PANEWINOFCLIENT(c->client);
    XChangeProperty(dpy, s->screen->rootid, desktop_atoms.clients,
                    XA_WINDOW, 32, PropModeReplace,
                    (unsigned char *)windows, (int)n);
    free(windows);
}

void
desktop_keys(DesktopScreen *s, Bool grab)
{
    Display *dpy = s->screen->dpy;
    const KeySym keys[] = {XK_Left, XK_Right, XK_Up, XK_Down};
    unsigned int ignore = ModMaskMap[MOD_IGNORE];
    /* Every subset of the configured ignore mask handles Caps/Num Lock. */
    for (size_t i = 0; i < sizeof keys / sizeof keys[0]; ++i) {
        KeyCode code = XKeysymToKeycode(dpy, keys[i]);
        if (!code)
            continue;
        unsigned int subset = ignore;
        for (;;) {
            unsigned int modifiers = ControlMask | Mod1Mask | subset;
            if (grab)
                XGrabKey(dpy, code, modifiers, s->screen->rootid, True,
                         GrabModeAsync, GrabModeAsync);
            else
                XUngrabKey(dpy, code, modifiers, s->screen->rootid);
            if (!subset)
                break;
            subset = (subset - 1) & ignore;
        }
    }
}

static Bool
desktop_key(XKeyEvent *key)
{
    DesktopScreen *s = desktop_screen(key->root);
    if (!s || s->columns * s->rows == 1 || key->send_event ||
        (key->state & ~ModMaskMap[MOD_IGNORE]) != (ControlMask | Mod1Mask))
        return False;
    KeySym symbol = XLookupKeysym(key, 0);
    long column = (long)(s->current % s->columns);
    long row = (long)(s->current / s->columns);
    switch (symbol) {
    case XK_Left: --column; break;
    case XK_Right: ++column; break;
    case XK_Up: --row; break;
    case XK_Down: ++row; break;
    default: return False;
    }
    if (key->type == KeyPress && column >= 0 && row >= 0 &&
        column < (long)s->columns && row < (long)s->rows)
        desktop_switch(s, (unsigned long)row * s->columns +
                       (unsigned long)column, key->time);
    return True;
}

Bool
virtual_desktop_event(Display *dpy, XEvent *event)
{
    if (event->type == KeyPress || event->type == KeyRelease)
        return desktop_key(&event->xkey);
    /* A legacy state transition may map a frame after updating WM_STATE. */
    if (event->type == MapNotify) {
        WinGeneric *window = WIGetInfo(event->xmap.window);
        if (window && window->core.client &&
            !virtual_desktop_visible(window->core.client))
            virtual_desktop_state(window->core.client);
        return False;
    }
    if (event->type != ClientMessage || event->xclient.format != 32)
        return False;
    XClientMessageEvent *message = &event->xclient;
    DesktopScreen *s = desktop_screen(message->window);
    DesktopClient *c = desktop_client(message->window);
    if (s && message->message_type == desktop_atoms.grid) {
        desktop_grid(s, (unsigned long)message->data.l[0],
                     (unsigned long)message->data.l[1]);
    } else if (s && message->message_type == desktop_atoms.current) {
        desktop_switch(s, (unsigned long)message->data.l[0],
                       (Time)message->data.l[1]);
    } else if (s && message->message_type == desktop_atoms.count) {
        unsigned long n = (unsigned long)message->data.l[0];
        if (n && n <= DESKTOP_MAX_COUNT) {
            /* Pick a rectangular grid with exactly the requested count. */
            for (unsigned long columns = DESKTOP_MAX_AXIS; columns; --columns)
                if (n % columns == 0 && n / columns <= DESKTOP_MAX_AXIS) {
                    desktop_grid(s, columns, n / columns);
                    break;
                }
        }
    } else if (c && message->message_type == desktop_atoms.desktop) {
        desktop_move(c, (unsigned long)message->data.l[0] & DESKTOP_ALL);
    } else if (c && message->message_type == desktop_atoms.activate) {
        if (c->desktop != DESKTOP_ALL)
            desktop_switch(c->screen, c->desktop, CurrentTime);
        StateNormal(c->client, CurrentTime);
        virtual_desktop_state(c->client);
        ClientFront(c->client);
        ClientSetFocus(c->client, True, CurrentTime);
    } else {
        return False;
    }
    XFlush(dpy);
    return True;
}
