/*
 * simple_menu.c -
 * Demonstrate the use of an XView menu in a canvas subwindow.
 * A Menu is brought up with the MENU mouse button.  The choices
 * in the menu toggle the display of the scrollbar next to the canvas.
 */
#include <xview/xview.h>
#include <xview/canvas.h>
#include <xview/scrollbar.h>

#define SCROLLBAR_KEY   100
#define MENU_KEY        200

int main(int argc, char    * argv[])
{
    Frame       frame;
    Canvas      canvas;
    Scrollbar   scrollbar;
    Menu        menu;
    void        menu_notify_proc(), pw_event_proc();

    xv_init(XV_INIT_ARGC_PTR_ARGV, &argc, argv, 0);

    /*
     *  Create a frame, canvas and menu.
     *  A canvas receives input in its canvas_paint_window().
     */
    frame = (Frame)xv_create(0, FRAME,
        FRAME_LABEL,    argv[0],
        0);
    canvas = (Canvas)xv_create(frame, CANVAS,
        XV_WIDTH,       300,
        XV_HEIGHT,      200,
        0);
    scrollbar = (Scrollbar)xv_create(canvas, SCROLLBAR,
        SCROLLBAR_DIRECTION,    SCROLLBAR_VERTICAL,
        0);

    menu = (Menu)xv_create(0, MENU,
        MENU_TITLE_ITEM,        "Scrollbar",
        MENU_STRINGS,           "On", "Off", 0,
        MENU_NOTIFY_PROC,       menu_notify_proc,
        XV_KEY_DATA,            SCROLLBAR_KEY, scrollbar,
        0);

    xv_set(canvas_paint_window(canvas),
        WIN_EVENT_PROC,         pw_event_proc,
        XV_KEY_DATA,            MENU_KEY, menu,
        0);

    window_fit(frame);
    window_main_loop(frame);
}

/*
 * menu_notify_proc - toggle the display of the scrollbar
 * based on which menu item was chosen.
 */
void
menu_notify_proc(Menu menu, Menu_item menu_item)
{
    char *menu_choice = (char *)xv_get(menu_item, MENU_STRING);
    int show_it = !strcmp(menu_choice, "On");

    xv_set(xv_get(menu, XV_KEY_DATA, SCROLLBAR_KEY),
        XV_SHOW,        show_it,
        0);
}

/*
 * Call menu_show() to display menu.
 */
void
pw_event_proc(Xv_Window canvas_pw, Event * event)
{
    if (event_action(event) == ACTION_MENU && event_is_down(event)) {
        Menu menu = (Menu)xv_get(canvas_pw, XV_KEY_DATA, MENU_KEY);
        menu_show(menu, canvas_pw, event, 0);
    }
}
