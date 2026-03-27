/*
 * Implements the xv deaf routines used by the XView misc module.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#include <xview_private/xv_deaf_.h>
#include <xview_private/gettext_.h>
#include <xview_private/windowutil_.h>
#include <stdio.h>
#include <X11/Xlib.h>
#include <xview_private/i18n_impl.h>
#include <xview_private/windowimpl.h>
#include <xview_private/draw_impl.h>
#include <xview/server.h>
#include <xview/screen.h>
#include <xview/window.h>
#include <xview/cursor.h>

/*
 * xv_deaf(parent, on)
 * makes the object 'parent' and it's descendants deaf if
 * 'on' is True. Otherwise, it restores their original input
 * state.
 */
int
xv_deaf(parent, on)
Xv_window	parent;
Bool		on;
{
    Xv_server   	server;
    Xv_screen   	screen;
    Xv_cursor		busyPointer = (Xv_cursor)NULL;

    screen = xv_get(parent, XV_SCREEN);
    server = xv_get(screen, SCREEN_SERVER);

    /*
     * Check if busy pointer has been created on server
     * already
     */
    busyPointer = xv_get(server, XV_KEY_DATA, (Attr_attribute)CURSOR_BUSY_PTR);

    /*
     * If it hasn't, create and store it on server as key data
     */
    if (!busyPointer) {
        busyPointer = xv_create(screen, CURSOR, 
				CURSOR_SRC_CHAR, OLC_BUSY_PTR, 
                                CURSOR_MASK_CHAR, OLC_BUSY_MASK_PTR,
                                NULL);
        xv_set(server, XV_KEY_DATA, (Attr_attribute)CURSOR_BUSY_PTR, busyPointer, NULL);
    }

    if (window_set_tree_flag(parent, busyPointer, TRUE, on) != XV_OK)  {
        xv_error(parent,
                ERROR_STRING, 
                XV_MSG("xv_deaf:attempt to make windows deaf/hear failed"), 
                NULL);
        return(XV_ERROR);
    }

    return(XV_OK);
}
