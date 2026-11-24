/* Private desktop state shared by the model and X11 protocol implementation.
 * The model owns these lists; Client and ScreenInfo pointers are borrowed. */
#ifndef OLWM_DESKTOP_PRIVATE_H
#define OLWM_DESKTOP_PRIVATE_H

#include <sys/types.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include "i18n.h"
#include "ollocale.h"
#include "olwm.h"
#include "win.h"
#include "desktop-protocol.h"

typedef struct DesktopScreen {
    ScreenInfo *screen;
    unsigned long columns, rows, current;
    struct DesktopScreen *next;
} DesktopScreen;

typedef struct DesktopClient {
    Client *client;
    DesktopScreen *screen;
    unsigned long desktop;
    Bool hidden;
    struct DesktopClient *next;
} DesktopClient;

typedef struct {
    Atom grid, version, activate, count, current, desktop, clients;
    Atom supported, geometry, viewport, workarea, roots;
} DesktopAtoms;

/* Module-owned lists, initialized once and released before olwm cleanup. */
extern DesktopScreen *desktop_screens;
extern DesktopClient *desktop_clients;
extern DesktopAtoms desktop_atoms;

/* Look up a borrowed screen record; return NULL for an unmanaged root. */
DesktopScreen *desktop_screen(Window root);
/* Look up a borrowed client record by pane XID; return NULL if untracked. */
DesktopClient *desktop_client(Window pane);

/* Read up to capacity CARDINALs into caller-owned values. Return the number
 * read, or zero for missing/malformed properties; frees Xlib storage. */
unsigned long desktop_read(Display *dpy, Window win, Atom atom,
                           unsigned long *values, unsigned long capacity);

/* Publish model state for the borrowed screen, including client mapping order.
 * Allocation failure preserves the previously published client list. */
void desktop_publish(DesktopScreen *screen);

/* Switch to a valid index, update visibility and choose a visible focus target.
 * Invalid indices are ignored; screen is borrowed. */
void desktop_switch(DesktopScreen *screen, unsigned long index, Time time);

/* Resize a borrowed screen's grid (1..4 per axis), moving removed desktops'
 * windows to the last remaining desktop. Invalid sizes are ignored. */
void desktop_grid(DesktopScreen *screen, unsigned long columns,
                  unsigned long rows);

/* Move a borrowed client and its dependent dialogs together. DESKTOP_ALL pins
 * the group; invalid indices are ignored. Also updates visibility and focus. */
void desktop_move(DesktopClient *client, unsigned long desktop);

/* Enable/disable Ctrl+Alt+arrow grabs on one screen. Includes lock modifiers;
 * grabs are removed in single-desktop mode. Does not own the screen. */
void desktop_keys(DesktopScreen *screen, Bool grab);

#endif
