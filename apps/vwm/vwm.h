/* Native desktop overview state. One application instance owns the model,
 * XView objects and drawing resources; client window IDs are borrowed. */
#ifndef OPENLOOK_VWM_H
#define OPENLOOK_VWM_H

#include <stddef.h>
#include <X11/Xlib.h>
#include <xview/xview.h>
#include <xview/frame.h>
#include <xview/canvas.h>
#include "desktop-protocol.h"

enum { VWM_CELL_WIDTH = 80, VWM_CELL_HEIGHT = 72 };

typedef struct {
    Window window;
    unsigned long desktop;
    int x, y, width, height;
    int iconic;
    char title[128];
} VwmWindow;

typedef struct {
    Display *display;
    Window root, own_window;
    Atom grid_atom, version_atom, current_atom, desktop_atom;
    Atom clients_atom, activate_atom, state_atom;
    unsigned long columns, rows, current;
    int screen_width, screen_height, available;
    VwmWindow *windows;
    size_t count;
} VwmModel;

typedef struct {
    VwmModel model;
    Frame frame;
    Canvas canvas;
    Xv_window paint;
    Display *display;
    GC gc;
    Pixmap buffer;
    unsigned long white, black, grey;
    int width, height, buffer_width, buffer_height;
    unsigned long grid_columns, grid_rows;
    int grid_ready, grid_pending, grid_adjusting;
    Window dragged;
    int press_x, press_y, press_desktop;
    Window last_click_window;
    unsigned long last_click_time;
} Vwm;

/* Open a separate query connection to the same display/screen as owner.
 * Model owns that connection; return zero on failure. */
int vwm_model_open(VwmModel *model, Xv_window owner);

/* Replace the model's owned snapshot from current WM properties. Returns
 * whether a compatible olwm is present; safely tolerates disappearing clients. */
int vwm_model_refresh(VwmModel *model);

/* Send a desktop request to olwm; target is root or a borrowed client XID.
 * No window is moved directly by the overview. */
void vwm_model_request(VwmModel *model, Window target, Atom action,
                       unsigned long first, unsigned long second);

/* Close the query connection, restore its error handler and free snapshots. */
void vwm_model_close(VwmModel *model);

/* Paint the overview from the current snapshot, allocating/resizing an owned
 * back buffer as necessary. */
void vwm_overview_draw(Vwm *vwm);

/* Canvas event callback: select/switch desktops, drag windows between them,
 * including clicks over miniatures; double-click activates, middle-click pins.
 * Borrows the callback objects. */
void vwm_overview_event(Xv_window window, Event *event);

/* Initialize fixed pane sizing and enable a 3x2 grid if starting with one
 * desktop. Borrows the initialized application's frame and model. */
void vwm_grid_init(Vwm *vwm);

/* Publish native resize increments, bounds and base size on the owned frame.
 * Retains unrelated hints so olwm can snap a resize to whole panes. */
void vwm_grid_hints(Vwm *vwm);

/* Canvas resize callback. Requests a grid of whole 80x72 panes (1..4 per axis);
 * frame snapping is deferred to refresh to avoid recursive toolkit layout. */
void vwm_grid_resize(Canvas canvas, int width, int height);

/* Reconcile pending grid requests and external grid changes after model refresh.
 * Snaps the owned frame to whole panes without resizing individual panes. */
void vwm_grid_refresh(Vwm *vwm);

/* Refresh the model, reconcile grid sizing and repaint the UI. */
void vwm_refresh(Vwm *vwm);

#endif
