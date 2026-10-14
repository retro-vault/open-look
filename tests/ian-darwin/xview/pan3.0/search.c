#include <errno.h>
#include <string.h>
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



int applysearch(), searchdone(), search_resize_proc();
Panel_item srchlist, srchapply;

static char re [MAXSEARCHLEN + 1];

int LLM_free(struct LLM_root * root);
int postsearch(struct LLM_root * root);
int trim(char * start);
int w_exposenote(struct Note * np, int display);
int w_matchingnotes(struct LLM_root * root, char * re, int notvisible);

int notesearch(item, event) /* don't use event without mods to search_menu_proc */
    Panel_item item;
    Event *event;
    {
    struct LLM_root root;
    struct Note *np;
    struct Note **npp;
    int  count;
    char buf [MAXSEARCHLEN + 1];

    count = 0;

    strcpy(buf, (char *) xv_get(item, PANEL_VALUE));
    trim(buf);
    if(strlen(buf) == 0)
        {
        notice_prompt(main_frame, 0,
            NOTICE_MESSAGE_STRINGS,
                "No search string entered",
                0,
            NOTICE_BUTTON_YES, "Acknowledge",
            NOTICE_NO_BEEPING, noticenobeep,
            0);
        return 0;
        }
    strcpy(re, buf);
    count = w_matchingnotes(&root, re, 1);
    if(count == -1)
        {
        notice_prompt(main_frame, 0,
            NOTICE_MESSAGE_STRINGS,
                "Invalid regular expression entered",
                0,
            NOTICE_BUTTON_YES, "Acknowledge",
            NOTICE_NO_BEEPING, noticenobeep,
            0);
        return 0;
        }
    if(count == -2)
        {
        notice_prompt(main_frame, 0,
            NOTICE_MESSAGE_STRINGS,
                "Memory allocation failure",
                strerror(errno),
                0,
            NOTICE_BUTTON_YES, "Acknowledge",
            NOTICE_NO_BEEPING, noticenobeep,
            0);
        LLM_free(&root);
        return 0;
        }
    if(count == 0)
        {
        notice_prompt(main_frame, 0,
            NOTICE_MESSAGE_STRINGS,
                "No hidden notes matching search string found",
                0,
            NOTICE_BUTTON_YES, "Acknowledge",
            NOTICE_NO_BEEPING, noticenobeep,
            0);
        return 0;
        }
    if(count == 1)
        {
        npp = (struct Note **) LLM_first(&root);
        np = *npp;
        w_exposenote(np, ERRONDISPLAY);
        }
    else
        {
        postsearch(&root);
        }
    LLM_free(&root);
    }

int refresh_popups(void);
int w_popupxy(int * x, int *y, int width, int height, int spacing);

int postsearch(struct LLM_root * root)
    {
    struct Note **npp;
    int  x, y;
    int  row;
    Panel search_panel;
    static XClassHint chint;

    w_popupxy(&x, &y, SEARCHWIDTH, SEARCHHEIGHT, SEARCHSPACING);
    search_frame = xv_create(main_frame, FRAME_CMD,
                           XV_LABEL, "Matching Notes",
                           XV_X, x,
                           XV_Y, y,
                           XV_WIDTH, SEARCHWIDTH,
                           XV_HEIGHT, SEARCHHEIGHT,
                           FRAME_NO_CONFIRM, TRUE,
                           FRAME_DONE_PROC, searchdone,
                           FRAME_SHOW_RESIZE_CORNER, TRUE,
                           WIN_EVENT_PROC, search_resize_proc,
                           WIN_CONSUME_EVENTS,
                               WIN_RESIZE,
                               0,
                           0);
    if(search_frame == 0)
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
    XSetClassHint((Display *)xv_get(search_frame, XV_DISPLAY),
        xv_get(search_frame, XV_XID), &chint);

    xv_set(search_frame, XV_SHOW, TRUE, 0);

    search_panel = (Panel) xv_get(search_frame, FRAME_CMD_PANEL);
    xv_set(search_panel, WIN_RETAINED, FALSE, 0);

    (void) xv_create(search_panel, PANEL_MESSAGE,
        XV_X, xv_col(search_panel, 0),
        XV_Y, xv_row(search_panel, 0),
        PANEL_LABEL_STRING, "Matching Notes",
        0);
    srchlist = xv_create(search_panel, PANEL_LIST,
        XV_X, xv_col(search_panel, 0),
        XV_Y, xv_row(search_panel, 1),
        PANEL_LIST_DISPLAY_ROWS, 7,
        PANEL_LIST_WIDTH, xv_col(search_panel, 25),
        PANEL_CHOOSE_ONE, FALSE,
        PANEL_READ_ONLY, TRUE,
        0);
    row = 0;
    npp = (struct Note **) LLM_first(root);
    while(npp != 0)
        {
        xv_set(srchlist,
            PANEL_LIST_INSERT, row,
            PANEL_LIST_STRING, row, (*npp)->ntitle, 
            PANEL_LIST_CLIENT_DATA, row, *npp, 
            0);
        npp = (struct Note **) LLM_next(root);
        row++;
        }


    srchapply = xv_create(search_panel, PANEL_BUTTON,
                 XV_X, SEARCHWIDTH / 2 - 30,
                 XV_Y, SEARCHHEIGHT - 30,
                 PANEL_LABEL_STRING, "Apply",
                 PANEL_NOTIFY_PROC, applysearch,
                 0);

    (void) xv_set(search_item, PANEL_INACTIVE, TRUE, 0);
    (void) xv_set(search_button, PANEL_INACTIVE, TRUE, 0);
    search_up = 1;
    }

int searchdone(void)
    {
    xv_destroy_safe(search_frame);
    (void) xv_set(search_item, PANEL_INACTIVE, FALSE, 0);
    (void) xv_set(search_button, PANEL_INACTIVE, FALSE, 0);
    (void) xv_set(main_panel, PANEL_CARET_ITEM, search_item, 0);
    search_up = 0;
    }

int applysearch(Panel_item item, Event * event)
    {
    struct Note *np;
    int  i, row;

    row = xv_get(srchlist, PANEL_LIST_NROWS);
    for(i = 0; i < row; i++)
        {
        if(xv_get(srchlist, PANEL_LIST_SELECTED, i))
            {
            np = (struct Note *) xv_get(srchlist, PANEL_LIST_CLIENT_DATA, i);
            if(np != 0) w_exposenote(np, ERRONDISPLAY);
            }
        }
    if(xv_get(search_frame, FRAME_CMD_PUSHPIN_IN) == FALSE)
        {
        xv_destroy_safe(search_frame);
        (void) xv_set(search_item, PANEL_INACTIVE, FALSE, 0);
        (void) xv_set(search_button, PANEL_INACTIVE, FALSE, 0);
        (void) xv_set(main_panel, PANEL_CARET_ITEM, search_item, 0);
        search_up = 0;
        }
    refresh_popups();
    }

int refresh_search(void)
    {
    struct LLM_root root;
    struct Note **npp;
    int  i;
    int  row;

    if(!search_up) return 0;

    xv_set(srchlist, XV_SHOW, FALSE, 0);

    row = xv_get(srchlist, PANEL_LIST_NROWS);
    for(i = row - 1; i >= 0; i--)
        {
        xv_set(srchlist, PANEL_LIST_DELETE, i, 0);
        }

    w_matchingnotes(&root, re, 1);
    row = 0;
    npp = (struct Note **) LLM_first(&root);
    while(npp != 0)
        {
        xv_set(srchlist,
            PANEL_LIST_INSERT, row,
            PANEL_LIST_STRING, row, (*npp)->ntitle,
            PANEL_LIST_CLIENT_DATA, row, *npp,
            0);
        npp = (struct Note **) LLM_next(&root);
        row++;
        }
    LLM_free(&root);

    xv_set(srchlist, XV_SHOW, TRUE, 0);
    }

int search_resize_proc(Frame frame, Event * event, Notify_arg arg)
    {
    Rect rect, brect, lrect;
    Rect *prect;
    int  rh, h;

    if(event_id(event) != WIN_RESIZE) return 0;
    frame_get_rect(frame, &rect);
    rect.r_height -= (topmargin + bottommargin); /* correct for wm borders */
    rect.r_width -= (leftmargin + rightmargin);

    prect = (Rect *) xv_get(srchapply, PANEL_ITEM_RECT);
    brect = *prect;
    xv_set(srchapply,
        XV_X, (rect.r_width / 2) - (brect.r_width / 2),
        XV_Y, rect.r_height - brect.r_height - DEFPANELSPACING,
        0);

    prect = (Rect *) xv_get(srchlist, PANEL_ITEM_RECT);
    lrect = *prect;
    prect = (Rect *) xv_get(srchapply, PANEL_ITEM_RECT);
    brect = *prect;
    rh = xv_get(srchlist, PANEL_LIST_ROW_HEIGHT);
    h = brect.r_top - lrect.r_top - (2 * rh);
    h = h / rh;
    if(h <= 0) h = 1;
    xv_set(srchlist,
        PANEL_LIST_WIDTH, rect.r_width - SCROLLWIDTH,
        PANEL_LIST_DISPLAY_ROWS, h,
        0);
    }

int search_menu_proc(Menu menu, Menu_item mitem)
    {
    char buf1[MAXSEARCHLEN + 1];
    char buf2[MAXSEARCHLEN + 1];

    strcpy(buf1, (char *) xv_get(search_item, PANEL_VALUE));
    strcpy(buf2, (char *) xv_get(mitem, MENU_STRING));
    xv_set(search_item, PANEL_VALUE, buf2, 0);
    notesearch(search_item, 0); /* event isn't used, so 0 is ok */
    xv_set(search_item, PANEL_VALUE, buf1, 0);
    }
