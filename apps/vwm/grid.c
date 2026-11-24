/* Fixed-size pager cells and ICCCM resize increments. The WM owns the grid;
 * requests are asynchronous and toolkit geometry changes stay outside layout
 * callbacks to avoid recursive resizing. */
#include "vwm.h"
#include <X11/Xutil.h>

static unsigned long
pane_count(int pixels, int increment)
{
    int count = pixels / increment;
    if (count < 1)
        count = 1;
    if (count > (int)DESKTOP_MAX_AXIS)
        count = DESKTOP_MAX_AXIS;
    return (unsigned long)count;
}

void
vwm_grid_hints(Vwm *vwm)
{
    XSizeHints hints = {0};
    long supplied;
    Window frame = (Window)xv_get(vwm->frame, XV_XID);
    XGetWMNormalHints(vwm->display, frame, &hints, &supplied);
    hints.flags |= PMinSize | PMaxSize | PBaseSize | PResizeInc;
    hints.min_width = hints.width_inc = VWM_CELL_WIDTH;
    hints.min_height = hints.height_inc = VWM_CELL_HEIGHT;
    hints.max_width = VWM_CELL_WIDTH * DESKTOP_MAX_AXIS;
    hints.max_height = VWM_CELL_HEIGHT * DESKTOP_MAX_AXIS;
    hints.base_width = hints.base_height = 0;
    XSetWMNormalHints(vwm->display, frame, &hints);
}

void
vwm_grid_init(Vwm *vwm)
{
    vwm->grid_columns = vwm->model.columns;
    vwm->grid_rows = vwm->model.rows;
    if (vwm->grid_columns * vwm->grid_rows == 1) {
        vwm->grid_columns = 3;
        vwm->grid_rows = 2;
        vwm->grid_pending = 1;
        vwm_model_request(&vwm->model, vwm->model.root,
            vwm->model.grid_atom, vwm->grid_columns, vwm->grid_rows);
    }
    vwm->grid_ready = 1;
    vwm_grid_refresh(vwm);
    vwm_grid_hints(vwm);
}

void
vwm_grid_resize(Canvas canvas, int width, int height)
{
    Vwm *vwm = (Vwm *)xv_get(canvas, WIN_CLIENT_DATA);
    if (!vwm || !vwm->grid_ready || vwm->grid_adjusting ||
        !vwm->model.available || width < 1 || height < 1)
        return;
    unsigned long columns = pane_count(width, VWM_CELL_WIDTH);
    unsigned long rows = pane_count(height, VWM_CELL_HEIGHT);
    if (columns == vwm->grid_columns && rows == vwm->grid_rows)
        return;
    vwm->grid_columns = columns;
    vwm->grid_rows = rows;
    vwm->grid_pending = 1;
    vwm->dragged = None;
    vwm->last_click_window = None;
    vwm_model_request(&vwm->model, vwm->model.root, vwm->model.grid_atom,
                       columns, rows);
}

void
vwm_grid_refresh(Vwm *vwm)
{
    if (!vwm->grid_ready || !vwm->model.available)
        return;
    if (vwm->model.columns == vwm->grid_columns &&
        vwm->model.rows == vwm->grid_rows)
        vwm->grid_pending = 0;
    if (!vwm->grid_pending) {
        vwm->grid_columns = vwm->model.columns;
        vwm->grid_rows = vwm->model.rows;
    }
    int width = (int)vwm->grid_columns * VWM_CELL_WIDTH;
    int height = (int)vwm->grid_rows * VWM_CELL_HEIGHT;
    if ((int)xv_get(vwm->frame, XV_WIDTH) != width ||
        (int)xv_get(vwm->frame, XV_HEIGHT) != height) {
        vwm->grid_adjusting = 1;
        xv_set(vwm->frame, XV_WIDTH, width, XV_HEIGHT, height, NULL);
        vwm->grid_adjusting = 0;
        vwm_grid_hints(vwm);
    }
}
