/*
 * Implements the hello sample application used by the XView app suite.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */

#include <stdlib.h>

#include <xview/canvas.h>
#include <xview/frame.h>
#include <xview/openmenu.h>
#include <xview/panel.h>
#include <xview/window.h>
#include <xview/xview.h>

static Xv_opaque
hello_quit(Menu menu, Menu_item item)
{
    Frame frame = (Frame)xv_get(menu, MENU_CLIENT_DATA);

    (void)item;
    xv_destroy_safe(frame);
    return XV_NULL;
}

static Xv_opaque
hello_about(Menu menu, Menu_item item)
{
    Frame frame = (Frame)xv_get(menu, MENU_CLIENT_DATA);

    (void)item;
    xv_set(frame, FRAME_LEFT_FOOTER, "hello (XView/Open Look)", NULL);
    return XV_NULL;
}

static void
hello_menu_button_proc(Panel_item item, Event *event)
{
    (void)item;
    (void)event;
}

int
main(int argc, char *argv[])
{
    Frame frame;
    Panel menubar;
    Canvas canvas;
    Menu file_menu;
    Menu help_menu;

    xv_init(XV_INIT_ARGC_PTR_ARGV, &argc, argv, NULL);

    frame = (Frame)xv_create(
        XV_NULL, FRAME,
        FRAME_LABEL, "hello",
        FRAME_SHOW_FOOTER, TRUE,
        FRAME_LEFT_FOOTER, "Use the File/Help menu bar at the top",
        XV_WIDTH, 520,
        XV_HEIGHT, 360,
        NULL
    );

    file_menu = (Menu)xv_create(
        XV_NULL, MENU_COMMAND_MENU,
        MENU_CLIENT_DATA, frame,
        MENU_ITEM,
            MENU_STRING, "Quit",
            MENU_NOTIFY_PROC, hello_quit,
            NULL,
        NULL
    );

    help_menu = (Menu)xv_create(
        XV_NULL, MENU_COMMAND_MENU,
        MENU_CLIENT_DATA, frame,
        MENU_ITEM,
            MENU_STRING, "About hello",
            MENU_NOTIFY_PROC, hello_about,
            NULL,
        NULL
    );

    menubar = (Panel)xv_create(
        frame, PANEL,
        PANEL_LAYOUT, PANEL_HORIZONTAL,
        XV_WIDTH, WIN_EXTEND_TO_EDGE,
        NULL
    );

    (void)xv_create(
        menubar, PANEL_BUTTON,
        PANEL_LABEL_STRING, "File",
        PANEL_NOTIFY_PROC, hello_menu_button_proc,
        PANEL_ITEM_MENU, file_menu,
        NULL
    );

    (void)xv_create(
        menubar, PANEL_BUTTON,
        PANEL_LABEL_STRING, "Help",
        PANEL_NOTIFY_PROC, hello_menu_button_proc,
        PANEL_ITEM_MENU, help_menu,
        NULL
    );

    window_fit_height(menubar);

    canvas = (Canvas)xv_create(
        frame, CANVAS,
        WIN_BELOW, menubar,
        XV_WIDTH, WIN_EXTEND_TO_EDGE,
        XV_HEIGHT, WIN_EXTEND_TO_EDGE,
        NULL
    );
    (void)canvas;

    xv_main_loop(frame);
    return EXIT_SUCCESS;
}
