/*
 * Screen-sized virtual desktops for olwm. Only decoration frames are hidden;
 * application mapping and ICCCM normal/iconic state remain independent.
 * This module owns side records rather than changing legacy client layouts.
 */
#include "desktop-private.h"
#include "virtual-desktop.h"
#include "client.h"
#include "group.h"
#include <X11/Xatom.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

DesktopScreen *desktop_screens;
DesktopClient *desktop_clients;
DesktopAtoms desktop_atoms;
extern List *ScreenInfoList;
extern XrmDatabase OlwmDB;

DesktopScreen *
desktop_screen(Window root)
{
    for (DesktopScreen *s = desktop_screens; s; s = s->next)
        if (s->screen->rootid == root)
            return s;
    return NULL;
}

DesktopClient *
desktop_client(Window pane)
{
    for (DesktopClient *c = desktop_clients; c; c = c->next)
        if (PANEWINOFCLIENT(c->client) == pane)
            return c;
    return NULL;
}

static DesktopClient *
client_record(const Client *client)
{
    for (DesktopClient *c = desktop_clients; c; c = c->next)
        if (c->client == client)
            return c;
    return NULL;
}

Bool
virtual_desktop_visible(const Client *client)
{
    DesktopClient *c = client_record(client);
    return !c || c->desktop == DESKTOP_ALL ||
           c->desktop == c->screen->current;
}

/* Never use IconHide here: that reparents/unmaps the application's icon pane. */
void
virtual_desktop_state(Client *client)
{
    DesktopClient *c = client_record(client);
    if (!c)
        return;
    Bool visible = virtual_desktop_visible(client);
    Display *dpy = client->dpy;
    if (!visible) {
        XUnmapWindow(dpy, client->framewin->core.self);
        if (client->iconwin)
            XUnmapWindow(dpy, client->iconwin->core.self);
    } else if (c->hidden) {
        if (client->wmState == NormalState)
            XMapWindow(dpy, client->framewin->core.self);
        else if (client->wmState == IconicState && client->iconwin &&
                 client->groupmask != GROUP_DEPENDENT)
            XMapWindow(dpy, client->iconwin->core.self);
    }
    c->hidden = !visible;
}

static void
publish_client(DesktopClient *c)
{
    XChangeProperty(c->client->dpy, PANEWINOFCLIENT(c->client),
                    desktop_atoms.desktop, XA_CARDINAL, 32, PropModeReplace,
                    (unsigned char *)&c->desktop, 1);
    virtual_desktop_state(c->client);
}

/* Transient ownership takes precedence over the application's group hint. */
static DesktopClient *
owner_record(Client *client)
{
    DesktopClient *owner = desktop_client(client->transientFor);
    if (!owner && client->groupmask == GROUP_DEPENDENT)
        owner = desktop_client(client->groupid);
    return owner;
}

void
virtual_desktop_add(Client *client)
{
    DesktopScreen *s = desktop_screen(client->scrInfo->rootid);
    if (!s || !client->framewin || (client->flags & CLOlwmOwned) ||
        client_record(client))
        return;
    DesktopClient *c = calloc(1, sizeof *c);
    if (!c) {
        fprintf(stderr, "olwm: cannot allocate desktop client record\n");
        return;
    }
    c->client = client;
    c->screen = s;
    c->desktop = s->current;
    unsigned long hint;
    if (desktop_read(client->dpy, PANEWINOFCLIENT(client),
                     desktop_atoms.desktop, &hint, 1) == 1 &&
        (hint == DESKTOP_ALL || hint < s->columns * s->rows))
        c->desktop = hint;
    DesktopClient *owner = owner_record(client);
    if (owner && owner->screen == s)
        c->desktop = owner->desktop;
    /* Append to retain EWMH's initial mapping order. */
    DesktopClient **tail = &desktop_clients;
    while (*tail)
        tail = &(*tail)->next;
    *tail = c;
    publish_client(c);
    desktop_publish(s);
    if (c->hidden)
        ClientFocusTopmost(client->dpy, client->scrInfo, CurrentTime);
}

void
virtual_desktop_forget(Client *client)
{
    DesktopClient **link = &desktop_clients;
    while (*link && (*link)->client != client)
        link = &(*link)->next;
    if (*link) {
        DesktopClient *old = *link;
        DesktopScreen *screen = old->screen;
        *link = old->next;
        free(old);
        desktop_publish(screen);
    }
}

void
desktop_switch(DesktopScreen *screen, unsigned long index, Time time)
{
    if (index >= screen->columns * screen->rows || index == screen->current)
        return;
    screen->current = index;
    for (DesktopClient *c = desktop_clients; c; c = c->next)
        if (c->screen == screen)
            virtual_desktop_state(c->client);
    desktop_publish(screen);
    ClientFocusTopmost(screen->screen->dpy, screen->screen, time);
}

/* Bound the owner walk to the number of clients so malformed cycles terminate. */
static Bool
belongs_to(DesktopClient *candidate, DesktopClient *parent)
{
    size_t limit = 0;
    for (DesktopClient *c = desktop_clients; c; c = c->next)
        ++limit;
    while (candidate && limit--) {
        if (candidate == parent)
            return True;
        candidate = owner_record(candidate->client);
    }
    return False;
}

void
desktop_move(DesktopClient *client, unsigned long desktop)
{
    DesktopScreen *s = client->screen;
    if (desktop != DESKTOP_ALL && desktop >= s->columns * s->rows)
        return;
    /* Moving a dialog moves its owner too, keeping modal windows reachable. */
    for (size_t i = 0; i < 64; ++i) {
        DesktopClient *owner = owner_record(client->client);
        if (!owner || owner == client || owner->screen != s)
            break;
        client = owner;
    }
    for (DesktopClient *c = desktop_clients; c; c = c->next) {
        if (c->screen == s && belongs_to(c, client)) {
            c->desktop = desktop;
            publish_client(c);
        }
    }
    ClientFocusTopmost(s->screen->dpy, s->screen, CurrentTime);
}

/* Preserve a pane's row and column as the grid changes. Removed panes merge
 * into the last remaining pane; pinned clients retain their sentinel. */
static unsigned long
resized_desktop(unsigned long desktop, unsigned long old_columns,
                unsigned long columns, unsigned long rows)
{
    if (desktop == DESKTOP_ALL)
        return DESKTOP_ALL;
    unsigned long column = desktop % old_columns;
    unsigned long row = desktop / old_columns;
    if (column >= columns || row >= rows)
        return columns * rows - 1;
    return row * columns + column;
}

void
desktop_grid(DesktopScreen *screen, unsigned long columns, unsigned long rows)
{
    if (!columns || columns > DESKTOP_MAX_AXIS ||
        !rows || rows > DESKTOP_MAX_AXIS)
        return;
    desktop_keys(screen, False);
    unsigned long old_columns = screen->columns;
    screen->current = resized_desktop(screen->current, old_columns,
                                      columns, rows);
    screen->columns = columns;
    screen->rows = rows;
    for (DesktopClient *c = desktop_clients; c; c = c->next) {
        if (c->screen != screen)
            continue;
        c->desktop = resized_desktop(c->desktop, old_columns, columns, rows);
        publish_client(c);
    }
    desktop_keys(screen, columns * rows > 1);
    desktop_publish(screen);
    ClientFocusTopmost(screen->screen->dpy, screen->screen, CurrentTime);
}

void
virtual_desktop_resize(ScreenInfo *screen)
{
    DesktopScreen *s = desktop_screen(screen->rootid);
    if (s)
        desktop_publish(s);
}

void
virtual_desktop_init(Display *dpy)
{
#define INTERN(field, name) desktop_atoms.field = XInternAtom(dpy, name, False)
    INTERN(grid, DESKTOP_GRID);
    INTERN(version, DESKTOP_VERSION);
    INTERN(activate, DESKTOP_ACTIVATE);
    INTERN(count, "_NET_NUMBER_OF_DESKTOPS");
    INTERN(current, "_NET_CURRENT_DESKTOP");
    INTERN(desktop, "_NET_WM_DESKTOP");
    INTERN(clients, "_NET_CLIENT_LIST");
    INTERN(supported, "_NET_SUPPORTED");
    INTERN(geometry, "_NET_DESKTOP_GEOMETRY");
    INTERN(viewport, "_NET_DESKTOP_VIEWPORT");
    INTERN(workarea, "_NET_WORKAREA");
    INTERN(roots, "_NET_VIRTUAL_ROOTS");
#undef INTERN
    /* Legacy GetUserDefaults reads files only. Also accept this new resource
     * from xrdb, without changing how any existing olwm resource is loaded. */
    const char *server_resources = XResourceManagerString(dpy);
    XrmDatabase server_db = server_resources ?
        XrmGetStringDatabase(server_resources) : NULL;
    List *list = ScreenInfoList;
    ScreenInfo *screen;
    while ((screen = ListEnum(&list)) != NULL) {
        DesktopScreen *s = calloc(1, sizeof *s);
        if (!s)
            continue;
        s->screen = screen;
        s->columns = s->rows = 1;
        unsigned long grid[2], current;
        char *type;
        XrmValue value;
        unsigned int columns, rows;
        char trailing;
        Bool configured = XrmGetResource(OlwmDB, "olwm.virtualDesktop",
                            "OpenWindows.VirtualDesktop", &type, &value);
        if (!configured && server_db)
            configured = XrmGetResource(server_db, "olwm.virtualDesktop",
                            "OpenWindows.VirtualDesktop", &type, &value);
        if (configured &&
            sscanf(value.addr, "%ux%u %c", &columns, &rows, &trailing) == 2 &&
            columns && columns <= DESKTOP_MAX_AXIS &&
            rows && rows <= DESKTOP_MAX_AXIS) {
            s->columns = columns;
            s->rows = rows;
        }
        /* Keep an interactive grid and current desktop through Restart WM. */
        if (desktop_read(dpy, screen->rootid, desktop_atoms.grid, grid, 2) == 2 &&
            grid[0] && grid[0] <= DESKTOP_MAX_AXIS &&
            grid[1] && grid[1] <= DESKTOP_MAX_AXIS) {
            s->columns = grid[0];
            s->rows = grid[1];
        }
        if (desktop_read(dpy, screen->rootid, desktop_atoms.current,
                         &current, 1) == 1 && current < s->columns * s->rows)
            s->current = current;
        s->next = desktop_screens;
        desktop_screens = s;
        desktop_publish(s);
        desktop_keys(s, s->columns * s->rows > 1);
    }
    if (server_db)
        XrmDestroyDatabase(server_db);
}

void
virtual_desktop_shutdown(Display *dpy)
{
    while (desktop_clients) {
        DesktopClient *c = desktop_clients;
        /* UnparentClient will remap client panes, including hidden desktops. */
        desktop_clients = c->next;
        free(c);
    }
    while (desktop_screens) {
        DesktopScreen *s = desktop_screens;
        desktop_keys(s, False);
        XDeleteProperty(dpy, s->screen->rootid, desktop_atoms.version);
        desktop_screens = s->next;
        free(s);
    }
}
