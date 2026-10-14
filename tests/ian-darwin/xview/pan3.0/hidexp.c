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
#include <stdlib.h>

#include "pan.h"



/*
    Routine called when hide notes button is selected.  Hides ALL notes.
*/
int refresh_destroy(void);
int refresh_move(void);
int refresh_popups(void);
int refresh_search(void);
int w_exposenote(struct Note * np, int display);
int w_hidenote(struct Note * np);
int w_veilnote(struct Note * np);

int hideall(void)
    {
    struct SubDir *sp;
    struct Note *np;

    sp = (struct SubDir *) LLM_first(&subdir_rt);
    while(sp != 0)
        {
        np = (struct Note *) LLM_first(&sp->note_rt);
        while(np != 0)
            {
            if(np->mapped && np->state != Hidden)
                {
                w_hidenote(np);
                }
            np = (struct Note *) LLM_next(&sp->note_rt);
            }
        sp = (struct SubDir *) LLM_next(&subdir_rt);
        }
    refresh_popups();
    }

/*
    Routine called when expose notes button menu selection All is selected.
    Exposes ALL notes.
*/
int exposeall(void)
    {
    int  count;
    struct SubDir *sp;
    struct Note *np;

    /* Count how many will be exposed */
    count = 0;
    sp = (struct SubDir *) LLM_first(&subdir_rt);
    while(sp != 0)
        {
        np = (struct Note *) LLM_first(&sp->note_rt);
        while(np != 0)
            {
            if(np->state != Visible)
                {
                if(!np->mapped)
                    {
                    count++;
                    }
                }
            np = (struct Note *) LLM_next(&sp->note_rt);
            }
        sp = (struct SubDir *) LLM_next(&subdir_rt);
        }
    if(windowmax != -1 && count >= windowmax)
        {
        char buf[1024];

        sprintf(buf, "This action will expose %d notes!", count);
        if(notice_prompt(main_frame, 0,
                     NOTICE_MESSAGE_STRINGS, 
                         buf,
                         "Proceed?",
                         0,
                     NOTICE_BUTTON_YES, "Yes",
                     NOTICE_BUTTON_NO, "No",
                     NOTICE_NO_BEEPING, noticenobeep,
                     0) == NOTICE_NO)
            {
            return 0;
            }
        }

    sp = (struct SubDir *) LLM_first(&subdir_rt);
    while(sp != 0)
        {
        np = (struct Note *) LLM_first(&sp->note_rt);
        while(np != 0)
            {
            if(np->state != Visible)
                {
                if(!np->mapped)
                    {
                    w_exposenote(np, ERRONDISPLAY);
                    }
                }
            np = (struct Note *) LLM_next(&sp->note_rt);
            }
        sp = (struct SubDir *) LLM_next(&subdir_rt);
        }
    refresh_popups();
    }

/*
    Routine called when expose notes button menu selection All is selected
    from within a folder.  Exposes ALL notes in the folder.
*/
int exposefolder(struct SubDir * sp)
    {
    int  count;
    struct Note *np;

    /* Count how many will be exposed */
    count = 0;
    np = (struct Note *) LLM_first(&sp->note_rt);
    while(np != 0)
        {
        if(np->state != Visible)
            {
            if(!np->mapped)
                {
                count++;
                }
            }
        np = (struct Note *) LLM_next(&sp->note_rt);
        }
    if(windowmax != -1 && count >= windowmax)
        {
        char buf[1024];
 
        sprintf(buf, "This action will expose %d notes!", count);
        if(notice_prompt(main_frame, 0,
                     NOTICE_MESSAGE_STRINGS,
                         buf,
                         "Proceed?",
                         0,
                     NOTICE_BUTTON_YES, "Yes",
                     NOTICE_BUTTON_NO, "No",
                     NOTICE_NO_BEEPING, noticenobeep,
                     0) == NOTICE_NO)
            {
            return 0;
            }
        }

    np = (struct Note *) LLM_first(&sp->note_rt);
    while(np != 0)
        {
        if(np->state != Visible)
            {
            if(!np->mapped)
                {
                w_exposenote(np, ERRONDISPLAY);
                }
            }
        np = (struct Note *) LLM_next(&sp->note_rt);
        }
    refresh_popups();
    }

/*
    Routine called when the hide button on a note is selected.  Hides the
    selected note.
*/
int hidenote(Panel_item item, Event * event)
    {
    struct Note *np;

    np = (struct Note *) xv_get(item, PANEL_CLIENT_DATA);

    w_hidenote(np);

    refresh_popups();
    }

/*
    Called when the window border menu Quit item is selected on a note.
    Veils the note.
*/
int dismissed(Frame frame)
    {
    int  found = 0;
    struct SubDir *sp;
    struct Note *np;

    sp = (struct SubDir *) LLM_first(&subdir_rt);
    while(sp != 0 && !found)
        {
        np = (struct Note *) LLM_first(&sp->note_rt);
        while(np != 0 && !found)
            {
            if(np->mapped && np->frame == frame)
                {
                w_veilnote(np);
                found = 1;
                }
            np = (struct Note *) LLM_next(&sp->note_rt);
            }
        sp = (struct SubDir *) LLM_next(&subdir_rt);
        }
    refresh_popups();
    }

/*
    Called when an expose menu item is chosen.  Figures out which note
    to expose, then does it.
*/
int exposemenu(Menu menu, Menu_item mitem)
    {
    struct SubDir *sp;
    struct Note *np;
    char *ptext;
    Menu_item pullright;

    np = (struct Note *)xv_get(mitem, MENU_CLIENT_DATA);
    if(np == 0 &&
       strcmp((char *)xv_get(mitem, MENU_STRING), CHOICE_ALL) == 0)
        {
        exposeall();
        }
    else if((long) np == 0xFFFFFFFF &&
       strcmp((char *)xv_get(mitem, MENU_STRING), CHOICE_ALL) == 0)
        {
        pullright = (Menu_item) xv_get(menu, MENU_PARENT);
        ptext = (char *)xv_get(pullright, MENU_STRING);
        sp = (struct SubDir *) LLM_first(&subdir_rt);
        while(sp != 0)
            {
            if(strcmp(sp->subdir, ptext) == 0)
                exposefolder(sp);
            sp = (struct SubDir *) LLM_next(&subdir_rt);
            }
        }
    else
        {
        if(np == 0)
            {
            notice_prompt(main_frame, 0,
                NOTICE_MESSAGE_STRINGS,
                    "No notes to expose",
                    0,
                NOTICE_BUTTON_YES, "Acknowledge",
                NOTICE_NO_BEEPING, noticenobeep,
                0);
            return 0;
            }
        if(!np->mapped)
            {
            w_exposenote(np, ERRONDISPLAY);
            }
        refresh_popups();
        }
    }

/* refresh any pop up windows as necessary */
int refresh_popups(void)
    {
    refresh_move();
    refresh_destroy();
    refresh_search();
    }
