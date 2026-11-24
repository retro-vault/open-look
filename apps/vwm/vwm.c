/* Native XView overview for olwm's optional desktops. Closing this app leaves
 * the desktops intact; resizing to one pane brings windows back together. */
#include "vwm.h"
#include "app-icon.h"
#include <X11/Xatom.h>
#include <xview/font.h>
#include <xview/notify.h>
#include <xview/cms.h>
#include <xview/xv_xrect.h>
#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* The application owns one frame and model for its main-loop lifetime. */
static Vwm app;

void
vwm_refresh(Vwm *vwm)
{
    vwm_model_refresh(&vwm->model);
    vwm_grid_refresh(vwm);
    vwm_overview_draw(vwm);
}

static Notify_value
tick(Notify_client client, int which)
{
    (void)client;
    (void)which;
    vwm_refresh(&app);
    return NOTIFY_DONE;
}

static void
repaint(Canvas canvas, Xv_window paint, Display *display, Window xid,
        Xv_xrectlist *rectangles)
{
    (void)canvas;
    (void)paint;
    (void)display;
    (void)xid;
    (void)rectangles;
    vwm_overview_draw(&app);
}

static unsigned long
colour(const char *name, unsigned long fallback)
{
    XColor colour, exact;
    Cms cms = xv_get(app.paint, WIN_CMS);
    Colormap map = cms ? (Colormap)xv_get(cms, CMS_CMAP_ID) :
        DefaultColormap(app.display, DefaultScreen(app.display));
    if (XAllocNamedColor(app.display, map, name, &colour, &exact))
        return colour.pixel;
    return fallback;
}

int
main(int argc, char **argv)
{
    xv_init(XV_INIT_ARGC_PTR_ARGV, &argc, argv, NULL);
    app.frame = xv_create(NULL, FRAME, FRAME_LABEL, "Virtual Desktops",
        FRAME_SHOW_FOOTER, FALSE, FRAME_NO_CONFIRM, TRUE,
        FRAME_MIN_SIZE, VWM_CELL_WIDTH, VWM_CELL_HEIGHT,
        FRAME_MAX_SIZE, VWM_CELL_WIDTH * DESKTOP_MAX_AXIS,
        VWM_CELL_HEIGHT * DESKTOP_MAX_AXIS,
        XV_WIDTH, 3 * VWM_CELL_WIDTH, XV_HEIGHT, 2 * VWM_CELL_HEIGHT,
        XV_SHOW, FALSE, NULL);
    if (!vwm_model_open(&app.model, app.frame)) {
        fprintf(stderr, "vwm: cannot open the desktop display\n");
        return 1;
    }
    if (!vwm_model_refresh(&app.model)) {
        fprintf(stderr, "vwm: run the updated olwm before opening the overview\n");
        vwm_model_close(&app.model);
        return 1;
    }
    app.display = (Display *)xv_get(app.frame, XV_DISPLAY);
    XWindowAttributes root_attributes;
    XGetWindowAttributes(app.display, app.model.root, &root_attributes);
    char selection_name[64];
    snprintf(selection_name, sizeof selection_name,
             "_OPENLOOK_DESKTOP_OVERVIEW_S%d",
             XScreenNumberOfScreen(root_attributes.screen));
    Atom selection = XInternAtom(app.display, selection_name, False);
    Window existing = XGetSelectionOwner(app.display, selection);
    if (existing != None) {
        vwm_model_request(&app.model, existing, app.model.activate_atom, 0, 0);
        vwm_model_close(&app.model);
        return 0;
    }
    XSetSelectionOwner(app.display, selection, app.model.own_window, CurrentTime);
    unsigned long all = DESKTOP_ALL;
    XChangeProperty(app.display, (Window)xv_get(app.frame, XV_XID),
        app.model.desktop_atom, XA_CARDINAL, 32, PropModeReplace,
        (unsigned char *)&all, 1);
    app_icon_apply(xv_get(app.frame, FRAME_ICON), "vwm");
    xv_set(xv_get(app.frame, FRAME_ICON), ICON_LABEL, "Desktops", NULL);
    app.canvas = xv_create(app.frame, CANVAS, CANVAS_X_PAINT_WINDOW, TRUE,
        OPENWIN_SHOW_BORDERS, FALSE,
        CANVAS_RETAINED, FALSE, CANVAS_AUTO_EXPAND, TRUE,
        CANVAS_AUTO_SHRINK, TRUE, CANVAS_REPAINT_PROC, repaint,
        WIN_CLIENT_DATA, &app, CANVAS_RESIZE_PROC, vwm_grid_resize,
        XV_X, 0, XV_Y, 0, NULL);
    app.paint = canvas_paint_window(app.canvas);
    xv_set(app.paint, WIN_CLIENT_DATA, &app, WIN_EVENT_PROC, vwm_overview_event,
        WIN_CONSUME_EVENTS, WIN_MOUSE_BUTTONS, LOC_DRAG,
        NULL, NULL);
    app.gc = XCreateGC(app.display, (Window)xv_get(app.paint, XV_XID), 0, NULL);
    Xv_Font font = xv_get(app.paint, WIN_FONT);
    XSetFont(app.display, app.gc, (Font)xv_get(font, XV_XID));
    app.black = colour("black", BlackPixel(app.display, 0));
    app.white = colour("white", WhitePixel(app.display, 0));
    app.grey = colour("#cccccc", app.white);
    vwm_grid_init(&app);
    struct itimerval timer = {{0, 250000}, {0, 250000}};
    notify_set_itimer_func(app.frame, tick, ITIMER_REAL, &timer, NULL);
    xv_set(app.frame, XV_SHOW, TRUE, NULL);
    /* XView may map/withdraw a frame while fitting its children. Apply the
     * sticky request after its final MapRequest is queued for olwm. */
    vwm_grid_hints(&app);
    XSync(app.display, False);
    vwm_model_request(&app.model, app.model.own_window,
                       app.model.desktop_atom, DESKTOP_ALL, 2);
    xv_main_loop(app.frame);
    if (app.buffer)
        XFreePixmap(app.display, app.buffer);
    XFreeGC(app.display, app.gc);
    vwm_model_close(&app.model);
    return 0;
}
