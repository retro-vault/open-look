#include <errno.h>
/*
Post A Note V3.0
Copyright (c) 1993, Jeffrey W. Bailey
All rights reserved.

Permission is granted to distribute this program in exact, complete
source form, which includes this copyright notice, as long as no fee
other than media and distribution cost is charged.

This program may not be used in whole, or in part, in any other manner
without prior written permission from the author.

This program may not be distributed in modified form without prior
written permission from the author.  In other words, patches may be
distributed, but modified source may not be distributed.

If there are any questions, comments or suggestions, the author may be
contacted at:

    jeff@rd1.interlan.com

    or

    Jeffrey Bailey
    Racal-Datacom, Inc.
    Mail Stop E-110
    1601 N. Harrison Parkway
    Sunrise, FL  33323-2899
*/
#include <stdio.h>
#include <stdlib.h>

#include "pan.h"
#include <X11/X.h>
#include <X11/Xutil.h>



int applymove();
int movedone();
int chosesrc();
int move_resize_proc();

static Panel_item srctext, dsttext, notetext, srclist, dstlist, notelist;
static Panel_item appbutton;

int refresh_popups(void);
int w_movenote(struct SubDir * src_sp, struct SubDir *dst_sp, struct Note * np, int display);
int w_popupxy(int * x, int *y, int width, int height, int spacing);

int movenote(Menu menu, Menu_item mitem)
    {
    Rect *prect;
    int  x, y;
    int  row;
    struct SubDir *sp;
    struct Note *np;
    static XClassHint chint;
    char buf[MAXTITLELEN + 1];

    w_popupxy(&x, &y, MOVEWIDTH, MOVEHEIGHT, MOVESPACING);
    move_mitem = mitem;
    move_frame = xv_create(main_frame, FRAME_CMD,
                           XV_LABEL, "Move Notes",
                           XV_X, x,
                           XV_Y, y,
                           FRAME_NO_CONFIRM, TRUE,
                           FRAME_DONE_PROC, movedone,
                           FRAME_SHOW_RESIZE_CORNER, TRUE,
                           WIN_EVENT_PROC, move_resize_proc,
                           WIN_CONSUME_EVENTS,
                               WIN_RESIZE,
                               0,
                           0);
    if(move_frame == 0)
        {
        notice_prompt(main_frame, 0,
            NOTICE_MESSAGE_STRINGS,
                "Unable to create sub-frame (internal error)",
                0,
            NOTICE_BUTTON_YES, "Acknowledge",
            NOTICE_NO_BEEPING, noticenobeep,
            0);
        return 0;
        }

    /* Set up the X class since xview doesn't */
    chint.res_name = "pan";
    chint.res_class = "Pan";
    XSetClassHint((Display *)xv_get(move_frame, XV_DISPLAY),
        xv_get(move_frame, XV_XID), &chint);

    move_panel = (Panel) xv_get(move_frame, FRAME_CMD_PANEL);
    xv_set(move_panel, WIN_RETAINED, FALSE, 0);

    srclist = xv_create(move_panel, PANEL_LIST,
        XV_X, xv_col(move_panel, 0),
        XV_Y, xv_row(move_panel, 1),
        PANEL_LIST_DISPLAY_ROWS, 7,
        PANEL_LIST_WIDTH, xv_col(move_panel, 15),
        PANEL_CHOOSE_ONE, TRUE,
        PANEL_READ_ONLY, TRUE,
        PANEL_NOTIFY_PROC, chosesrc,
        0);
    srctext = xv_create(move_panel, PANEL_MESSAGE,
        XV_X, xv_col(move_panel, 0),
        XV_Y, xv_row(move_panel, 0),
        PANEL_LABEL_STRING, "Source Folder",
        0);

    prect = (Rect *) xv_get(srclist, PANEL_ITEM_RECT);
    dstlist = xv_create(move_panel, PANEL_LIST,
        XV_X, prect->r_left + prect->r_width + SCROLLWIDTH + 20,
        XV_Y, xv_row(move_panel, 1),
        PANEL_LIST_DISPLAY_ROWS, 7,
        PANEL_LIST_WIDTH, xv_col(move_panel, 15),
        PANEL_CHOOSE_ONE, TRUE,
        PANEL_READ_ONLY, TRUE,
        0);
    dsttext = xv_create(move_panel, PANEL_MESSAGE,
        XV_X, prect->r_left + prect->r_width + SCROLLWIDTH + 20,
        XV_Y, xv_row(move_panel, 0),
        PANEL_LABEL_STRING, "Destination Folder",
        0);

    row = 0;
    sp = (struct SubDir *) LLM_first(&subdir_rt);
    while(sp != 0)
        {
        xv_set(srclist,
            PANEL_LIST_INSERT, row,
            PANEL_LIST_STRING, row, sp->subdir, 
            PANEL_LIST_CLIENT_DATA, row, sp, 
            0);
        xv_set(dstlist,
            PANEL_LIST_INSERT, row,
            PANEL_LIST_STRING, row, sp->subdir, 
            PANEL_LIST_CLIENT_DATA, row, sp, 
            0);
        sp = (struct SubDir *) LLM_next(&subdir_rt);
        row++;
        }

    prect = (Rect *) xv_get(dstlist, PANEL_ITEM_RECT);
    notelist = xv_create(move_panel, PANEL_LIST,
        XV_X, prect->r_left + prect->r_width + SCROLLWIDTH + 20,
        XV_Y, xv_row(move_panel, 1),
        PANEL_LIST_DISPLAY_ROWS, 7,
        PANEL_LIST_WIDTH, xv_col(move_panel, 20),
        PANEL_CHOOSE_ONE, FALSE,
        PANEL_READ_ONLY, TRUE,
        0);

    notetext = xv_create(move_panel, PANEL_MESSAGE,
        XV_X, prect->r_left + prect->r_width + SCROLLWIDTH + 20,
        XV_Y, xv_row(move_panel, 0),
        PANEL_LABEL_STRING, "Notes To Move",
        0);

    y = prect->r_top + prect->r_height + DEFPANELSPACING;

    appbutton = xv_create(move_panel, PANEL_BUTTON,
                     XV_X, xv_col(move_panel, 0),
                     XV_Y, y,
                     PANEL_LABEL_STRING, "Apply",
                     PANEL_NOTIFY_PROC, applymove,
                     0);

    window_fit(move_panel);
    window_fit(move_frame);

    row = 0;
    sp = (struct SubDir *) LLM_first(&subdir_rt);
    np = (struct Note *)LLM_first(&sp->note_rt);
    while(np != 0)
        {
        strcpy(buf, np->ntitle);
        if(strlen(buf) == 0) strcpy(buf, NOTITLE);
        xv_set(notelist,
            PANEL_LIST_INSERT, row,
            PANEL_LIST_STRING, row, buf,
            PANEL_LIST_CLIENT_DATA, row, np, 
            PANEL_LIST_SELECT, row, FALSE,
            0);
        np = (struct Note *)LLM_next(&sp->note_rt);
        row++;
        }
    xv_set(move_frame, XV_SHOW, TRUE, 0);
    (void) xv_set(move_mitem, MENU_INACTIVE, TRUE, 0);
    move_up = 1;
    }

int movedone(void)
    {
    xv_destroy_safe(move_frame);
    (void) xv_set(move_mitem, MENU_INACTIVE, FALSE, 0);
    move_up = 0;
    }

int applymove(Panel_item item, Event * event)
    {
    int  i;
    int  row;
    struct Note *np;
    struct SubDir *src_sp;
    struct SubDir *dst_sp;

    /* get src dir */
    row = xv_get(srclist, PANEL_LIST_NROWS);
    for(i = 0; i < row; i++)
        {
        if(xv_get(srclist, PANEL_LIST_SELECTED, i))
            {
            src_sp = (struct SubDir *) xv_get(srclist, PANEL_LIST_CLIENT_DATA,
                                              i);
            }
        }

    /* get dst dir */
    row = xv_get(dstlist, PANEL_LIST_NROWS);
    for(i = 0; i < row; i++)
        {
        if(xv_get(dstlist, PANEL_LIST_SELECTED, i))
            {
            dst_sp = (struct SubDir *) xv_get(dstlist, PANEL_LIST_CLIENT_DATA,
                                              i);
            }
        }

    row = xv_get(notelist, PANEL_LIST_NROWS);
    for(i = 0; i < row; i++)
        {
        if(xv_get(notelist, PANEL_LIST_SELECTED, i))
            {
            np = (struct Note *) xv_get(notelist, PANEL_LIST_CLIENT_DATA, i);
            if(w_movenote(src_sp, dst_sp, np, ERRONDISPLAY) < 0)
                {
                if(xv_get(move_frame, FRAME_CMD_PUSHPIN_IN) == FALSE)
                    {
                    xv_destroy_safe(move_frame);
                    (void) xv_set(move_mitem, MENU_INACTIVE, FALSE, 0);
                    move_up = 0;
                    }
                return 0;
                }
            }
        }

    xv_set(notelist, XV_SHOW, FALSE, 0);
    for(i = row - 1; i >= 0; i--)
        {
        if(xv_get(notelist, PANEL_LIST_SELECTED, i))
            {
            xv_set(notelist, PANEL_LIST_DELETE, i, 0);
            }
        }
    xv_set(notelist, XV_SHOW, TRUE, 0);

    if(xv_get(move_frame, FRAME_CMD_PUSHPIN_IN) == FALSE)
        {
        xv_destroy_safe(move_frame);
        (void) xv_set(move_mitem, MENU_INACTIVE, FALSE, 0);
        move_up = 0;
        }
    refresh_popups();
    }

int chosesrc(Panel_item item, char * string, char * client_data, Panel_list_op op, Event * event)
    {
    int  i;
    int  row;
    struct SubDir *sp;
    struct Note *np;
    char buf[MAXTITLELEN + 1];

    if(op == PANEL_LIST_OP_SELECT)
        {
        xv_set(notelist, XV_SHOW, FALSE, 0);
        row = xv_get(notelist, PANEL_LIST_NROWS);
        for(i = row - 1; i >= 0; i--)
            xv_set(notelist, PANEL_LIST_DELETE, i, 0);
        sp = (struct SubDir *)client_data;
        row = 0;
        np = (struct Note *)LLM_first(&sp->note_rt);
        while(np != 0)
            {
            strcpy(buf, np->ntitle);
            if(strlen(buf) == 0) strcpy(buf, NOTITLE);
            xv_set(notelist,
                PANEL_LIST_INSERT, row,
                PANEL_LIST_STRING, row, buf,
                PANEL_LIST_CLIENT_DATA, row, np, 
                PANEL_LIST_SELECT, row, FALSE,
                0);
            np = (struct Note *)LLM_next(&sp->note_rt);
            row++;
            }
        xv_set(notelist, XV_SHOW, TRUE, 0);
        }
    }

int refresh_move(void)
    {
    int  row, i;
    struct SubDir *sp;
    struct Note *np;
    char buf[MAXTITLELEN + 1];

    if(!move_up) return 0;

    xv_set(srclist, XV_SHOW, FALSE, 0);
    xv_set(dstlist, XV_SHOW, FALSE, 0);
    xv_set(notelist, XV_SHOW, FALSE, 0);

    row = xv_get(srclist, PANEL_LIST_NROWS);
    for(i = row - 1; i >= 0; i--)
        xv_set(srclist, PANEL_LIST_DELETE, i, 0);

    row = xv_get(dstlist, PANEL_LIST_NROWS);
    for(i = row - 1; i >= 0; i--)
        xv_set(dstlist, PANEL_LIST_DELETE, i, 0);

    row = xv_get(notelist, PANEL_LIST_NROWS);
    for(i = row - 1; i >= 0; i--)
        xv_set(notelist, PANEL_LIST_DELETE, i, 0);

    row = 0;
    sp = (struct SubDir *) LLM_first(&subdir_rt);
    while(sp != 0)
        {
        xv_set(srclist,
            PANEL_LIST_INSERT, row,
            PANEL_LIST_STRING, row, sp->subdir, 
            PANEL_LIST_CLIENT_DATA, row, sp, 
            0);
        xv_set(dstlist,
            PANEL_LIST_INSERT, row,
            PANEL_LIST_STRING, row, sp->subdir, 
            PANEL_LIST_CLIENT_DATA, row, sp, 
            0);
        sp = (struct SubDir *) LLM_next(&subdir_rt);
        row++;
        }

    row = 0;
    sp = (struct SubDir *) LLM_first(&subdir_rt);
    np = (struct Note *)LLM_first(&sp->note_rt);
    while(np != 0)
        {
        strcpy(buf, np->ntitle);
        if(strlen(buf) == 0) strcpy(buf, NOTITLE);
        xv_set(notelist,
            PANEL_LIST_INSERT, row,
            PANEL_LIST_STRING, row, buf,
            PANEL_LIST_CLIENT_DATA, row, np, 
            PANEL_LIST_SELECT, row, FALSE,
            0);
        np = (struct Note *)LLM_next(&sp->note_rt);
        row++;
        }

    xv_set(srclist, XV_SHOW, TRUE, 0);
    xv_set(dstlist, XV_SHOW, TRUE, 0);
    xv_set(notelist, XV_SHOW, TRUE, 0);
    }

int move_resize_proc(Frame frame, Event * event, Notify_arg arg)
    {
    Rect rect, *prect;
    int  x, y, rh, h;

    if(event_id(event) != WIN_RESIZE) return 0;
    frame_get_rect(frame, &rect);
    rect.r_height -= (topmargin + bottommargin); /* correct for wm title bar */
    rect.r_width -= (leftmargin + rightmargin);  /* correct for wm border */

    prect = (Rect *) xv_get(appbutton, PANEL_ITEM_RECT);
    y = rect.r_height - prect->r_height - DEFPANELSPACING;
    x = rect.r_width / 2 - prect->r_width / 2;
    xv_set(appbutton, XV_Y, y, XV_X, x, 0);

    rh = xv_get(srclist, PANEL_LIST_ROW_HEIGHT);
    y -= (2 * rh);
    prect = (Rect *) xv_get(srclist, PANEL_ITEM_RECT);
    y -= prect->r_top;

    if(y < rh) y = rh;
    h = y / rh;

    xv_set(srclist, PANEL_LIST_DISPLAY_ROWS, h, 0);
    xv_set(dstlist, PANEL_LIST_DISPLAY_ROWS, h, 0);
    xv_set(notelist, PANEL_LIST_DISPLAY_ROWS, h, 0);

    prect = (Rect *) xv_get(notelist, PANEL_ITEM_RECT);
    y = rect.r_width - prect->r_left - SCROLLWIDTH;
    xv_set(notelist, PANEL_LIST_WIDTH, y, 0);
    }
