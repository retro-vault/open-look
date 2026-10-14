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
#include <unistd.h>

#include "pan.h"
#include <X11/X.h>
#include <X11/Xutil.h>



int applydestroy(), destroydone();
Panel_item fldrlist;

int rf_resize_proc();

int LLM_delete(struct LLM_root * root, char * node);
int LLM_free(struct LLM_root * root);
int put_win(struct Note * np);
int refresh_popups(void);
int w_popupxy(int * x, int *y, int width, int height, int spacing);

int destroyfolder(Menu menu, Menu_item mitem)
    {
    Rect rect, *prect;
    struct SubDir *sp;
    int  x, y;
    int  row;
    Panel destroy_panel;
    static XClassHint chint;

    w_popupxy(&x, &y, DESTROYWIDTH, DESTROYHEIGHT, DESTROYSPACING);
    destroy_mitem = mitem;
    destroy_frame = xv_create(main_frame, FRAME_CMD,
                           XV_LABEL, "Destroy Folder",
                           XV_X, x,
                           XV_Y, y,
                           FRAME_NO_CONFIRM, TRUE,
                           FRAME_DONE_PROC, destroydone,
                           WIN_EVENT_PROC, rf_resize_proc,
                           WIN_CONSUME_EVENTS,
                               WIN_RESIZE,
                               0,
                           0);
    if(destroy_frame == 0)
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
    XSetClassHint((Display *)xv_get(destroy_frame, XV_DISPLAY),
        xv_get(destroy_frame, XV_XID), &chint);

    destroy_panel = (Panel) xv_get(destroy_frame, FRAME_CMD_PANEL);
    xv_set(destroy_panel, WIN_RETAINED, FALSE, 0);

    (void) xv_create(destroy_panel, PANEL_MESSAGE,
        XV_X, xv_col(destroy_panel, 0),
        XV_Y, xv_row(destroy_panel, 0),
        PANEL_LABEL_STRING, "Destroy Folder",
        0);
    fldrlist = xv_create(destroy_panel, PANEL_LIST,
        XV_X, xv_col(destroy_panel, 0),
        XV_Y, xv_row(destroy_panel, 1),
        PANEL_LIST_DISPLAY_ROWS, 7,
        PANEL_CHOOSE_ONE, TRUE,
        PANEL_READ_ONLY, TRUE,
        0);
    row = 0;
    sp = (struct SubDir *) LLM_first(&subdir_rt);
    while(sp != 0)
        {
        xv_set(fldrlist,
            PANEL_LIST_INSERT, row,
            PANEL_LIST_STRING, row, sp->subdir, 
            PANEL_LIST_CLIENT_DATA, row, sp, 
            0);
        sp = (struct SubDir *) LLM_next(&subdir_rt);
        row++;
        }

    prect = (Rect *) xv_get(fldrlist, PANEL_ITEM_RECT);

    destroy_item =  xv_create(destroy_panel, PANEL_BUTTON,
                 XV_Y, prect->r_top + prect->r_height + 20,
                 PANEL_LABEL_STRING, "Apply",
                 PANEL_NOTIFY_PROC, applydestroy,
                 0);

    window_fit(destroy_panel);
    window_fit(destroy_frame);

    xv_set(destroy_frame, XV_SHOW, TRUE, 0);

    frame_get_rect(destroy_frame, &rect);
    prect = (Rect *) xv_get(destroy_item, PANEL_ITEM_RECT);
    xv_set(destroy_item,
        XV_X, ((rect.r_width / 2) - (prect->r_width / 2)),
        0);
    xv_set(fldrlist, PANEL_LIST_WIDTH, rect.r_width - SCROLLWIDTH, 0);

    (void) xv_set(mitem, MENU_INACTIVE, TRUE, 0);
    destroy_up = 1;
    }

int destroydone(void)
    {
    xv_destroy_safe(destroy_frame);
    (void) xv_set(destroy_mitem, MENU_INACTIVE, FALSE, 0);
    destroy_up = 0;
    }

int applydestroy(Panel_item item, Event * event)
    {
    Menu_item mitem;
    struct SubDir *sp;
    struct Note *np;
    int  i, row;
    DIR *dp;
    struct dirent *ent;
    char buf[MAXBUFLEN];

    row = xv_get(fldrlist, PANEL_LIST_NROWS);
    for(i = 0; i < row; i++)
        {
        if(xv_get(fldrlist, PANEL_LIST_SELECTED, i))
            {
            sp = (struct SubDir *) xv_get(fldrlist, PANEL_LIST_CLIENT_DATA, i);
            }
        }

#ifdef CONFIDENT
    if(confirmdestroy)
        {
#endif
        if(notice_prompt(destroy_frame, 0,
                     NOTICE_MESSAGE_STRINGS, "Really destroy this folder?",
                         sp->subdir,
                         "(This action will destroy all notes in the folder)",
                         0,
                     NOTICE_BUTTON_YES, "Yes",
                     NOTICE_BUTTON_NO, "No",
                     NOTICE_NO_BEEPING, noticenobeep,
                     0) == NOTICE_NO)
            {
            if(xv_get(destroy_frame, FRAME_CMD_PUSHPIN_IN) == FALSE)
                {
                xv_destroy_safe(destroy_frame);
                (void) xv_set(destroy_mitem, MENU_INACTIVE, FALSE, 0);
                destroy_up = 0;
                }
            return 0;
            }
#ifdef CONFIDENT
        }
#endif

    np = (struct Note *) LLM_first(&sp->note_rt);
    while(np != 0)
        {
        if(np->mapped)
            {
            textsw_reset(np->textsw, 0, 0);
            put_win(np);
            np->mapped = 0;
            }
        np = (struct Note *) LLM_next(&sp->note_rt);
        }

    dp = opendir(sp->subdir);
    if(dp == 0)
        {
        notice_prompt(main_frame, 0,
            NOTICE_MESSAGE_STRINGS,
                "Couldn't read directory",
                sp->subdir,
                strerror(errno),
                0,
            NOTICE_BUTTON_YES, "Acknowledge",
            NOTICE_NO_BEEPING, noticenobeep,
            0);
        if(xv_get(destroy_frame, FRAME_CMD_PUSHPIN_IN) == FALSE)
            {
            xv_destroy_safe(destroy_frame);
            (void) xv_set(destroy_mitem, MENU_INACTIVE, FALSE, 0);
            destroy_up = 0;
            }
        return 0;
        }
    while((ent = readdir(dp)) != 0)
        {
        if(strcmp(ent->d_name, ".") != 0 && strcmp(ent->d_name, "..") != 0)
            {
            sprintf(buf, "%s/%s", sp->subdir, ent->d_name);
            unlink(buf);
            }
        }
    closedir(dp);
    if(rmdir(sp->subdir) < 0)
        {
        notice_prompt(main_frame, 0,
            NOTICE_MESSAGE_STRINGS,
                "Couldn't remove directory",
                sp->subdir,
                strerror(errno),
                0,
            NOTICE_BUTTON_YES, "Acknowledge",
            NOTICE_NO_BEEPING, noticenobeep,
            0);
        }

    mitem = xv_find(main_newnote, MENUITEM, MENU_STRING, sp->subdir, 0);
    if(mitem != 0)
        {
        (void) xv_set(main_newnote, MENU_REMOVE_ITEM, mitem, 0);
        xv_destroy_safe(mitem);
        }
    mitem = xv_find(main_expose, MENUITEM, MENU_STRING, sp->subdir, 0);
    if(mitem != 0)
        {
        (void) xv_set(main_expose, MENU_REMOVE_ITEM, mitem, 0);
        xv_destroy_safe(mitem);
        }
    mitem = xv_find(main_print, MENUITEM, MENU_STRING, sp->subdir, 0);
    if(mitem != 0)
        {
        (void) xv_set(main_print, MENU_REMOVE_ITEM, mitem, 0);
        xv_destroy_safe(mitem);
        }

    LLM_free(&sp->note_rt);
    LLM_delete(&subdir_rt, (char *)sp);

    refresh_popups();
    if(xv_get(destroy_frame, FRAME_CMD_PUSHPIN_IN) == FALSE)
        {
        xv_destroy_safe(destroy_frame);
        (void) xv_set(destroy_mitem, MENU_INACTIVE, FALSE, 0);
        destroy_up = 0;
        }
    }

int refresh_destroy(void)
    {
    int  row, i;
    struct SubDir *sp;

    if(!destroy_up) return 0;

    xv_set(fldrlist, XV_SHOW, FALSE, 0);

    row = xv_get(fldrlist, PANEL_LIST_NROWS);
    for(i = row - 1; i >= 0; i--)
        xv_set(fldrlist, PANEL_LIST_DELETE, i, 0);

    row = 0;
    sp = (struct SubDir *) LLM_first(&subdir_rt);
    while(sp != 0)
        {
        xv_set(fldrlist,
            PANEL_LIST_INSERT, row,
            PANEL_LIST_STRING, row, sp->subdir, 
            PANEL_LIST_CLIENT_DATA, row, sp, 
            0);
        sp = (struct SubDir *) LLM_next(&subdir_rt);
        row++;
        }

    xv_set(fldrlist, XV_SHOW, TRUE, 0);
    }

/*
    Center the button here even though we don't allow a resize because of
    the intermittent failure of frame_get_rect() immediately after frame
    creation.  Also adjust the list size.
*/
int rf_resize_proc(Frame frame, Event * event, Notify_arg arg)
    {
    Rect rect, *dprect, *prect;
 
    if(event_id(event) != WIN_RESIZE) return 0;
    frame_get_rect(frame, &rect);
    rect.r_height -= (topmargin + bottommargin); /* correct for wm title bar */
    rect.r_width -= (leftmargin + rightmargin);
 
    prect = (Rect *) xv_get(fldrlist, PANEL_ITEM_RECT);
    dprect = (Rect *) xv_get(destroy_item, PANEL_ITEM_RECT);
    xv_set(destroy_item,
                 XV_Y, prect->r_top + prect->r_height + 20,
                 XV_X, rect.r_width / 2 - dprect->r_width / 2,
                 0);
    xv_set(fldrlist, PANEL_LIST_WIDTH, rect.r_width - SCROLLWIDTH, 0);
    }
