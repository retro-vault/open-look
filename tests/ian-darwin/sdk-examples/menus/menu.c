/*
 * xv_menu.c -
 *      Demonstrate the use of an XView menu in a canvas subwindow.
 *      Menu is brought up with right mouse button and displays
 *      choice made in the canvas.  Allow menu to be pinned.
 */
#include <xview/xview.h>
#include <xview/canvas.h>

Frame   frame;

int main(int argc, char    * argv[])
{
    Canvas      canvas;
    Menu        menu;
    void        my_notify_proc(), my_event_proc();
    extern void exit();

    xv_init(XV_INIT_ARGC_PTR_ARGV, &argc, argv, 0);

    frame = (Frame)xv_create(0, FRAME,
        FRAME_LABEL,    argv[0],
        0);
    canvas = (Canvas)xv_create(frame, CANVAS,
        XV_WIDTH,       300,
        XV_HEIGHT,      200,
        0);
    menu = (Menu)xv_create(0, MENU,
        MENU_TITLE_ITEM,        "Junk",
        MENU_STRINGS,           "Yes", "No", "Maybe", 0,
        MENU_NOTIFY_PROC,       my_notify_proc,
        MENU_ITEM,
            MENU_STRING,        "Save",
            MENU_NOTIFY_PROC,   my_notify_proc,
            MENU_PULLRIGHT,
                xv_create(canvas, MENU,
                    MENU_GEN_PIN_WINDOW,        frame, "Save",
                    MENU_ITEM,
                        MENU_STRING,            "Update Changes",
                        MENU_NOTIFY_PROC,       my_notify_proc,
                        0,
                    0),
            0,
        MENU_ITEM,
            MENU_STRING,        "Quit",
            MENU_NOTIFY_PROC,   exit,
            0,
        0);

    xv_set(canvas_paint_window(canvas),
        WIN_CONSUME_EVENTS,     WIN_MOUSE_BUTTONS, 0,
        WIN_EVENT_PROC,         my_event_proc,
        /* associate the menu to the canvas win so we can retreive it easily */
        WIN_CLIENT_DATA,        menu,
        0);

    window_fit(frame);
    window_main_loop(frame);
}

/*
 * my_notify_proc - Display menu selection in frame header.
 */
void
my_notify_proc(Menu menu, Menu_item menu_item)
{
    xv_set(frame,
        FRAME_LABEL,    xv_get(menu_item, MENU_STRING),
        0);
}

/*
 * my_event_proc - Call menu_show() to display menu on right mouse button push.
 */
void
my_event_proc(Xv_Window window, Event * event)
{
    if (event_action(event) == ACTION_MENU && event_is_down(event)) {
        Menu menu = (Menu)xv_get(window, WIN_CLIENT_DATA);
        menu_show(menu, window, event, 0);
    }
}
